#include "mytcpserver.h"
#include <QDebug>
MyTcpServer::MyTcpServer() { }

MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}

void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "new client connected";
    MyTcpSocket *pTcpSocket = new MyTcpSocket;
    pTcpSocket->setSocketDescriptor(socketDescriptor);
    m_tcpSocketList.append(pTcpSocket);
    connect(pTcpSocket,SIGNAL(Offline(MyTcpSocket*)),this,SLOT(deleteSocket(MyTcpSocket*)));
}

MyTcpSocket *MyTcpServer::getMyTcpSocket(const char *name)
{
    for(auto it  = m_tcpSocketList.begin();it!=m_tcpSocketList.end();it++){
        if(0 == strcmp((*it)->getName().toStdString().c_str(),name)){
            return (*it);
        }
    }
    return NULL;
}

void MyTcpServer::sendAllUser(PDU *pdu)
{
    // char caName[32] = {'\0'};
    // strncpy(caName,pdu->caData,32);
    for(auto it = m_tcpSocketList.begin(); it != m_tcpSocketList.end();it++) {
        qDebug() << (*it)->getName() << " "  << (char*)pdu->caMsg;
        // if(0 != strcmp((*it)->getName().toStdString().c_str(),caName)){
        (*it)->write((char*)pdu,pdu->uiPDULen);
        // }
    }
}

void MyTcpServer::deleteSocket(MyTcpSocket *mySocket)
{
    for(auto it = m_tcpSocketList.begin();it != m_tcpSocketList.end();it++){
        // qDebug() << (*it)->getName();
        if(*it == mySocket) {
            (*it)->deleteLater();
            *it = NULL;
            m_tcpSocketList.erase(it);
            break;
        }
    }
    for(auto socket : m_tcpSocketList){
        qDebug() << socket->getName();
    }
}
