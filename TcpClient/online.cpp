#include "online.h"
#include "tcpclient.h"
#include "ui_online.h"

Online::Online(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}

void Online::showAllOnlineUser(PDU *pdu)
{
    if(NULL == pdu) {
        return ;
    }
    char caTemp[32];
    uint uisize = pdu->uiMsgLen/32;
    ui->listWidget->clear();
    for(uint i = 0; i < uisize; i++){
        memcpy(caTemp,(char*)(pdu->caMsg)+i*32,32);
        ui->listWidget->addItem(caTemp);

    }
}

void Online::on_pushButton_clicked()
{
    QString friendName = ui->listWidget->currentItem()->text();
    if(friendName.isEmpty()){
        return ;
    }
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
    strncpy(pdu->caData,friendName.toStdString().c_str(),friendName.size());
    qDebug() << pdu->caData;
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

