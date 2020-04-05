#include "robo_token.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    robo_token w;
    w.show();
    return a.exec();
}
