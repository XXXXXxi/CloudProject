#include "tcpserver.h"
#include "ui_tcpserver.h"

#include "mytcpserver.h"

#include <qfile.h>
#include <qmessagebox.h>

TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);

}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/server.config");
    if(file.open(QIODevice::ReadOnly)){
        QByteArray baData =  file.readAll();
        QString strData = baData.toStdString().c_str();
        // qDebug() << strData;
        file.close();
        strData.replace("\r\n"," ");
        // qDebug() << strData;
        QStringList strList = strData.split(" ");
        // for(int i = 0;i<strList.size();i++){
        //     qDebug() << strList[i] ;
        // }
        m_strIP = strList.at(0);;
        m_usPort = strList.at(1).toUShort();
        qDebug() << "ip:" << m_strIP << " prot:" << m_usPort ;
    }else{
        QMessageBox::critical(this,"open config","open config failed!");
    }

}
