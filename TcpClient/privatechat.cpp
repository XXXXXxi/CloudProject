#include "privatechat.h"
#include "TcpClient.h"
#include "protocol.h"
#include "ui_privatechat.h"

#include <QMessageBox>

PrivateChat::PrivateChat(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PrivateChat)
{
    ui->setupUi(this);
}

PrivateChat::~PrivateChat()
{
    delete ui;
}

PrivateChat &PrivateChat::getInstance()
{
    static PrivateChat instance;
    return instance;
}

void PrivateChat::setSendToName(QString name)
{
    sendToName = name;
}

void PrivateChat::showMsg(PDU *pdu)
{
    char caName[32] = {'\0'};
    strncpy(caName,pdu->caData,32);
    setSendToName(caName);
    if(this->isHidden()){
        this->show();
    }
    QString Msg = QString("%1 send : %2").arg(caName).arg((char*)pdu->caMsg);
    ui->listWidget->addItem(Msg);
}

void PrivateChat::on_pushButton_clicked()
{
    if(!ui->lineEdit->text().isEmpty()) {
        QString Msg = ui->lineEdit->text();
        PDU *pdu = mkPDU(Msg.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;
        strncpy(pdu->caData,sendToName.toStdString().c_str(),32);
        strncpy((char*)pdu->caMsg,Msg.toStdString().c_str(),Msg.size());
        qDebug() << sendToName << " " << pdu->caMsg;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

    }else{
        QMessageBox::warning(this,"发送消息","发送消息不能为空");
    }
}

