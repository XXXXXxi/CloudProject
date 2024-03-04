#include "protocol.h"
#include "tcpclient.h"
#include "ui_tcpclient.h"
#include "optwidget.h"
#include <QMessageBox>
void TcpClient::showConnect()
{
    // qDebug() << " 1" ;
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

void TcpClient::recvMsg()
{
    if(!m_bDownload){
        qDebug() << m_tcpSocket.bytesAvailable();
        uint uiPDULen = 0;
        m_tcpSocket.read((char*)&uiPDULen,sizeof(uint));
        uint uiMsgLen = uiPDULen-sizeof(PDU);
        PDU *pdu = mkPDU(uiMsgLen);
        m_tcpSocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));

        // qDebug() << pdu->uiMsgType << " " << (char*)(pdu->caMsg);
        switch(pdu->uiMsgType){
        case ENUM_MSG_TYPE_REGIST_RESPOND:{
            if(0 == strcmp(pdu->caData,REGIST_OK)){
                QMessageBox::information(this,"注册","注册成功");
            }else if(0 == strcmp(pdu->caData,REGIST_FAILED)){
                QMessageBox::warning(this,"注册","注册失败");
            }
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_RESPOND:{
            if(0 == strcmp(pdu->caData,LOGIN_OK)){
                QMessageBox::information(this,"登录","登录成功");
                OptWidget::getInstance().show();
                this->hide();
            }else if(0 == strcmp(pdu->caData,LOGIN_FAILED)){
                QMessageBox::warning(this,"登录","登录失败");
            }
            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:{
            OptWidget::getInstance().getFriend()->showAllOnlineUser(pdu);
            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USER_RESPOND:{
            QMessageBox::information(this,"搜索用户",QString(pdu->caData));
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
            OptWidget::getInstance().getFriend()->addFriendRequest(pdu);
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
            OptWidget::getInstance().getFriend()->addFriendRespond(pdu);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:{
            OptWidget::getInstance().getFriend()->showFlushFriend(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:{
            OptWidget::getInstance().getFriend()->showDeleteInfo(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:{
            OptWidget::getInstance().getFriend()->showDeleteInfo(pdu);
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:{
            PrivateChat::getInstance().showMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:{
            OptWidget::getInstance().getFriend()->showGroupCharMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:{
            OptWidget::getInstance().getFile()->showCreateFileMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_DIR_RESPOND:{
            OptWidget::getInstance().getFile()->showFlushList(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DELETE_DIR_RESPOND:{
            OptWidget::getInstance().getFile()->showDeleteDirMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_RENAME_DIR_RESPOND:{
            OptWidget::getInstance().getFile()->showRenameDirMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:{
            OptWidget::getInstance().getFile()->showUploadFileMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FILE_RESPOND:{
            OptWidget::getInstance().getFile()->showDeleteFileMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:{
            OptWidget::getInstance().getFile()->recvDownlodFileInfo(pdu);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:{
            OptWidget::getInstance().getFile()->recvShareFileInfo(pdu);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE:{
            char *pPath = new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            char *pos = strrchr(pPath,'/');
            if(NULL != pos) {
                pos++;
                QString strNote = QString("%1 share file -> %2").arg(pdu->caData).arg(pos);
                int ret = QMessageBox::question(this,"共享文件",strNote);
                if(QMessageBox::Yes == ret) {
                    PDU *respdu = mkPDU(pdu->uiMsgLen);
                    respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;
                    memcpy((char*)respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
                    QString strName = TcpClient::getInstance().getStrName();
                    strcpy(respdu->caData,strName.toStdString().c_str());
                    m_tcpSocket.write((char*)respdu,respdu->uiPDULen);
                }
            }

            break;
        }
        // case ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA:{
        //     OptWidget::getInstance().getFile()->recvDownloadFileData(pdu);
        //     break;
        // }
        default:
            break;
        }
        free(pdu);
        pdu = NULL;
    }else{
        QByteArray buff = m_tcpSocket.readAll();
        m_file.write(buff);
        m_iRecved += buff.size();
        qDebug() << m_iRecved;
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
        qDebug() << m_iTotal << " " << m_iRecved;
        if(m_iTotal == m_iRecved) {
            m_file.close();
            m_bDownload = false;
        }else if(m_iTotal < m_iRecved){
            m_file.close();
            m_bDownload = false;
        }
    }

}

TcpClient::TcpClient(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClient)
{
    ui->setupUi(this);
    loadConfig();
    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    qDebug() << " 1" ;
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_usPort);
}

TcpClient::~TcpClient()
{
    delete ui;
}

void TcpClient::loadConfig()
{
    QFile file(":/client.config");
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

TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}

QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}

QString TcpClient::getStrName()
{
    return strName;
}

QString TcpClient::getCurrPath()
{
    return currPath;
}

QString TcpClient::setCurrPath(QString name)
{
    currPath = name;
}

void TcpClient::setbDownload(bool status)
{
    m_bDownload = status;
}

void TcpClient::setiTotal(quint64 size)
{
    m_iTotal = size;
}

void TcpClient::setmFile(QString path)
{

    m_file.setFileName(path);
    m_file.open(QIODevice::WriteOnly);
}

void TcpClient::setiRecved(quint64 size)
{
    m_iRecved = size;
}

// void TcpClient::on_send_pb_clicked()
// {
    // QString strMsg = ui->lineEdit->text();
    // if(!strMsg.isEmpty()){
    //     PDU *pdu = mkPDU(strMsg.size()+1);
    //     pdu->uiMsgType = 8888;
    //     memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());
    //     qDebug() << (char*)(pdu->caMsg);
    //     m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
    //     free(pdu);
    //     pdu = NULL;
    // }else{
    //     QMessageBox::warning(this,"信息发送","发送信息不能为空");
    // }
// }


void TcpClient::on_login_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty()){
        this->strName = strName;
        this->currPath =QString("./%1").arg(strName);
        qDebug() << currPath ;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        QMessageBox::critical(this,"登录","登录失败");
    }
}


void TcpClient::on_register_pb_clicked()
{
    QString strName = ui->name_le->text();
    QString strPwd = ui->pwd_le->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty()){
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caData,strName.toStdString().c_str(),32);
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);
        m_tcpSocket.write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        QMessageBox::critical(this,"注册","用户名或密码输入为空");
    }
}


void TcpClient::on_cancel_pb_clicked()
{

}

