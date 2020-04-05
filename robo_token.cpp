#include "robo_token.h"
#include "ui_robo_token.h"

robo_token::robo_token(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::robo_token)
{
    ui->setupUi(this);

    QWidget::setWindowTitle(QWidget::windowTitle() + " 0.0.1");

    m_udp_socket = nullptr;

    connect(ui->m_minus, SIGNAL(released()), this, SLOT(next_interface_minus()));
    connect(ui->m_plus, SIGNAL(released()), this, SLOT(next_interface_plus()));
    connect(ui->m_refresh, SIGNAL(released()), this, SLOT(refresh()));
    connect(ui->m_clear, SIGNAL(released()), this, SLOT(clear()));
    connect(ui->m_send, SIGNAL(released()), this, SLOT(send_broadcast()));

    clear();
    refresh();
    next_interface_minus();
}


robo_token::~robo_token()
{
    delete ui;
}

void robo_token::refresh(void)
{
    QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    m_addr.clear();

    for(auto addr : addrs)
    {
        if((addr.protocol() == QAbstractSocket::NetworkLayerProtocol::IPv4Protocol) && (addr != QHostAddress(QHostAddress::LocalHost)))
        {
            m_addr.push_back(addr.toString());
        }
    }
    ui->m_netif_found->setText(QString::number(m_addr.size()) + " found");
    ui->m_minus->setDisabled((m_addr.size() <= 1));
    ui->m_plus->setDisabled((m_addr.size() <= 1));
}

void robo_token::next_interface_minus()
{
    next_interface(false);
}


void robo_token::next_interface_plus()
{
    next_interface(true);
}

void robo_token::next_interface(bool plus)
{
   if(plus)
        m_addrs_index++;
    else
        m_addrs_index--;

    if(m_addrs_index < 0)
        m_addrs_index = m_addr.size() -1 ;
    else if(m_addrs_index >=m_addr.size())
        m_addrs_index = 0;

    ui->m_interface->setText(m_addr.at(m_addrs_index));
}

void robo_token::clear(void)
{
    ui->m_token_text->clear();
    ui->m_token_text->setText("Answer:\n");
}

void robo_token::read_datagram(void)
{
    QHostAddress host;
    quint16 port;
    QByteArray datagram;
    while (m_udp_socket->hasPendingDatagrams())
    {
        datagram.resize(int(m_udp_socket->pendingDatagramSize()));
        m_udp_socket->readDatagram(datagram.data(), datagram.size(), &host, &port);
    }

    QString str;
    if(datagram.size() > 16)
    {
        datagram.remove(0, datagram.size()-16);
        qDebug() << "size " << datagram.size();
        qDebug() << "datagram " << datagram;

        str = "Answer:\n";
        str += "RAW data:\t" + datagram + "\n";
        str +="Token:\t" + QString(datagram).toLatin1().toHex() + "\n";
        str +="IP Adress:\t" + host.toString();

        if(str.size() && host.toString() != ui->m_interface->text())
            ui->m_token_text->setText(str);
    }
}

void robo_token::send_broadcast(void)
{
    clear();

    QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    bool found = false;
    QHostAddress a;
    for(auto addr : addrs)
    {
        if(addr.toString() == ui->m_interface->text() && (addr.protocol() == QAbstractSocket::NetworkLayerProtocol::IPv4Protocol) && (addr != QHostAddress(QHostAddress::LocalHost)))
        {
            found = true;
            a = addr;
            break;
        }
    }

    if(found)
    {
        QByteArray datagram = QByteArray::fromHex("21310020ffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        if(m_udp_socket != nullptr)
        {
            disconnect(m_udp_socket);
            delete m_udp_socket;
        }

        m_udp_socket = new QUdpSocket(this);
        m_udp_socket->bind(a, 54321);
        connect(m_udp_socket, SIGNAL(readyRead()),this, SLOT(read_datagram()));
        m_udp_socket->writeDatagram(datagram, a.Broadcast, 54321);
    }
}
