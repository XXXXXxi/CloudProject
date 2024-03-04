#include "friend.h"
#include "protocol.h"

#include <QMessageBox>
#include <TcpClient.h>

Friend::Friend(QWidget *parent)
    : QWidget{parent}
{
    m_pShowMsgTe = new QTextEdit;
    m_pFrinendListWidget = new QListWidget;
    m_pInputMsgLE = new QLineEdit;

    m_pDelFriendPB = new QPushButton("删除好友");
    m_pFlushFriendPB = new QPushButton("刷新好友");
    m_pShowOnlineUserPB = new QPushButton("显示在线好友");
    m_pSearchUserPB = new QPushButton("查找用户");

    m_pMsgSendPB = new QPushButton("信息发送");
    m_pPrivateChatPB = new QPushButton("私聊");

    QVBoxLayout *pRightVBL = new QVBoxLayout;
    pRightVBL->addWidget(m_pDelFriendPB);
    pRightVBL->addWidget(m_pFlushFriendPB);
    pRightVBL->addWidget(m_pShowOnlineUserPB);
    pRightVBL->addWidget(m_pSearchUserPB);
    pRightVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTe);
    pTopHBL->addWidget(m_pFrinendListWidget);
    pTopHBL->addLayout(pRightVBL);

    m_pOnline = new Online;

    QHBoxLayout *pMsgHBL = new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);

    QVBoxLayout *pMain = new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();

    setLayout(pMain);

    connect(m_pShowOnlineUserPB,SIGNAL(clicked(bool)),this,SLOT(showOnline()));
    connect(m_pSearchUserPB,SIGNAL(clicked(bool)),this,SLOT(searchUser()));
    connect(m_pFlushFriendPB,SIGNAL(clicked(bool)),this,SLOT(flushFriend()));
    connect(m_pDelFriendPB,SIGNAL(clicked(bool)),this,SLOT(deleteFriend()));
    connect(m_pPrivateChatPB,SIGNAL(clicked(bool)),this,SLOT(privateChat()));
    connect(m_pMsgSendPB,SIGNAL(clicked(bool)),this,SLOT(groupChat()));
}

void Friend::showAllOnlineUser(PDU *pdu)
{
    if(NULL == pdu) {
        return ;
    }
    m_pOnline->showAllOnlineUser(pdu);
}

void Friend::addFriendRequest(PDU *pdu)
{
    char caName[32] = {'\0'};
    strncpy(caName,pdu->caData,32);
    int ret = QMessageBox::question(this,"添加好友",QString("%1 request to make friend").arg(caName),QMessageBox::Yes,QMessageBox::No);
    PDU *respdu = mkPDU(0);
    respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
    if(ret == QMessageBox::Yes){
        strncpy(respdu->caData,caName,32);
    }else if(ret == QMessageBox::No){
        strncpy(respdu->caData,ADD_FAILED,32);
    }
    TcpClient::getInstance().getTcpSocket().write((char*)respdu,respdu->uiPDULen);
    free(respdu);
    respdu = NULL;
}

void Friend::addFriendRespond(PDU *pdu)
{
    QMessageBox::information(this,"添加好友",QString("%1").arg(pdu->caData));
}

void Friend::showFlushFriend(PDU *pdu)
{
    uint size = pdu->uiMsgLen/32;
    m_pFrinendListWidget->clear();
    for(uint i = 0 ; i < size; i++){
        char caName[32];
        strncpy(caName,(char*)(pdu->caMsg)+i*32,32);
        qDebug() << caName ;
        m_pFrinendListWidget->addItem(caName);
    }
}

void Friend::showDeleteInfo(PDU *pdu)
{
    if(pdu->uiMsgType == ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND) {
        QMessageBox::information(this,"删除好友","删除好友成功");
    }else if(pdu->uiMsgType == ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST) {
        QMessageBox::information(this,"删除好友",QString("%1 删除好友").arg(pdu->caData));
    }
}

void Friend::showGroupCharMsg(PDU *pdu)
{
    m_pShowMsgTe->append(QString("%1 send : %2").arg(pdu->caData).arg((char*)pdu->caMsg));
}

QListWidget *Friend::getFriendList()
{
    return m_pFrinendListWidget;
}


void Friend::showOnline()
{
    if(m_pOnline->isHidden()){
        m_pOnline->show();
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        m_pOnline->hide();
    }
}

void Friend::searchUser()
{
    QString name = QInputDialog::getText(this,"查找用户","请输入用户名:");
    if(!name.isEmpty()){
        // qDebug() << name ;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USER_REQUEST;
        strncpy((char*)pdu->caData,name.toStdString().c_str(),32);
        qDebug() << pdu->caData;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }

}

void Friend::flushFriend()
{
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;

}

void Friend::deleteFriend()
{
    if(NULL !=m_pFrinendListWidget->currentItem()){
        QString name = m_pFrinendListWidget->currentItem()->text();
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
        strncpy(pdu->caData,name.toStdString().c_str(),32);
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }

}

void Friend::privateChat()
{
    if(NULL != m_pFrinendListWidget->currentItem()) {
        QString name =m_pFrinendListWidget->currentItem()->text();
        if(PrivateChat::getInstance().isHidden()){
            PrivateChat::getInstance().show();
        }
        PrivateChat::getInstance().setSendToName(name);
    }
}

void Friend::groupChat()
{
    if(!m_pInputMsgLE->text().isEmpty()) {
        QString msg = m_pInputMsgLE->text();
        PDU *pdu = mkPDU(msg.size()+1);
        strncpy((char*)pdu->caMsg,msg.toStdString().c_str(),msg.size());
        pdu->uiMsgType = ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
        qDebug() << (char*)pdu->caMsg ;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu= NULL;
    }else {
        QMessageBox::warning(this,"群发信息","群发信息不能为空");
    }
}
