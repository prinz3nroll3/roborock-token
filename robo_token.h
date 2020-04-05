#ifndef ROBO_TOKEN_H
#define ROBO_TOKEN_H

#include <QMainWindow>
#include <QNetworkInterface>
#include <QUdpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class robo_token; }
QT_END_NAMESPACE

class robo_token : public QMainWindow
{
    Q_OBJECT
    public:
        robo_token(QWidget *parent = nullptr);
        ~robo_token();

    private slots:
        void next_interface_plus(void);
        void next_interface_minus(void);
        void refresh(void);
        void send_broadcast(void);
        void read_datagram(void);
        void clear(void);
    private:     
        void next_interface(bool plus);
        Ui::robo_token *ui;
        int m_addrs_index;
        QStringList m_addr;
        QUdpSocket* m_udp_socket;
};
#endif // ROBO_TOKEN_H
