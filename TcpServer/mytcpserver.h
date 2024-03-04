#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include "MyTcpSocket.h"
#include <QTcpServer>

class MyTcpServer : public QTcpServer
{
    Q_OBJECT
public:
    MyTcpServer();
    static MyTcpServer&getInstance();
    virtual void incomingConnection(qintptr socketDescriptor);
    MyTcpSocket* getMyTcpSocket(const char* name);
    void sendAllUser(PDU *pdu);
public slots:
    void deleteSocket(MyTcpSocket* mySocket);
private:
    QList<MyTcpSocket*> m_tcpSocketList;
};

#endif // MYTCPSERVER_H
