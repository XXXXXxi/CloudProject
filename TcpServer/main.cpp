#include "tcpserver.h"

#include "DBOperator.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpServer w;
    DBOperator::getInstance();
    w.show();
    return a.exec();
}
