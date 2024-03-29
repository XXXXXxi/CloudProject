#include "optwidget.h"
#include "sharefile.h"

#include <QButtonGroup>
#include <QCheckBox>
#include <QHBoxLayout>
#include <string.h>

ShareFile::ShareFile(QWidget *parent)
    : QWidget{parent}
{
    m_pSelectAllPB = new QPushButton("全选");
    m_pCancelSelectPB = new QPushButton("取消选择");

    m_pOKPB = new QPushButton("确定");
    m_pCancelPB = new QPushButton("取消");

    m_pSA = new QScrollArea;
    m_pFriendW = new QWidget;
    m_pFriendVBL = new QVBoxLayout(m_pFriendW);
    m_pButtonGroup = new QButtonGroup(m_pFriendW);
    m_pButtonGroup->setExclusive(false);

    QHBoxLayout *pTopHBL = new QHBoxLayout;
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancelSelectPB);
    pTopHBL->addStretch();

    QHBoxLayout *pDownHBL = new QHBoxLayout;
    pDownHBL->addWidget(m_pOKPB);
    pDownHBL->addWidget(m_pCancelPB);

    QVBoxLayout *pMainVBL = new QVBoxLayout;
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pSA);
    pMainVBL->addLayout(pDownHBL);

    setLayout(pMainVBL);

    connect(m_pCancelSelectPB,SIGNAL(clicked(bool)),this,SLOT(cancelSelect()));
    connect(m_pSelectAllPB,SIGNAL(clicked(bool)),this,SLOT(selectAll()));
    connect(m_pOKPB,SIGNAL(clicked(bool)),this,SLOT(shareSuccess()));
    connect(m_pCancelPB,SIGNAL(clicked(bool)),this,SLOT(shareCancel()));

    // test();
}

ShareFile &ShareFile::getInstance()
{
    static ShareFile instance;
    return instance;
}

void ShareFile::test()
{
    QVBoxLayout *p = new QVBoxLayout(m_pFriendW);
    QCheckBox *pCB = NULL;
    for(int i = 0; i<15 ;i++){
        pCB = new QCheckBox("jack");
        p->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}

void ShareFile::updateFriend(QListWidget *pFriendList)
{
    if(NULL == pFriendList) {
        return ;
    }
    QList<QAbstractButton*> preFriendList = m_pButtonGroup->buttons();
    for(int i = 0;i<preFriendList.size();i++){
        m_pFriendVBL->removeWidget(preFriendList[i]);
        m_pButtonGroup->removeButton(preFriendList[i]);
        delete preFriendList[i];
        preFriendList[i] = NULL;
    }
    QCheckBox *pCB = NULL;
    for(int i = 0; i<pFriendList->count() ;i++){
        qDebug() << pFriendList->item(i)->text();
        pCB = new QCheckBox(pFriendList->item(i)->text());
        m_pFriendVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}

void ShareFile::cancelSelect()
{
    QList<QAbstractButton*> cblist = m_pButtonGroup->buttons();
    for(int i = 0 ;i < cblist.size();i++) {
        if(cblist[i]->isChecked()){
            cblist[i]->setChecked(false);
        }
    }
}

void ShareFile::selectAll()
{
    QList<QAbstractButton*> cblist = m_pButtonGroup->buttons();
    for(int i = 0 ;i < cblist.size();i++) {
        if(!cblist[i]->isChecked()){
            cblist[i]->setChecked(true);
        }
    }
}

void ShareFile::shareSuccess()
{
    QString strName = TcpClient::getInstance().getStrName();
    QString strCurPath = TcpClient::getInstance().getCurrPath();
    QString strShareFileName = OptWidget::getInstance().getFile()->getStrShareFileName();

    QString strPath = strCurPath + "/" + strShareFileName;

    QList<QAbstractButton*> cblist = m_pButtonGroup->buttons();
    int num = 0;
    for(int i = 0 ;i < cblist.size();i++) {
        if(cblist[i]->isChecked()){
            num++;
        }
    }

    PDU *pdu = mkPDU(32*num+strPath.size());
    pdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->caData,"%s %d",strName.toStdString().c_str(),num);
    int j = 0;
    for(int i = 0;i<cblist.size();i++) {
        if(cblist[i]->isChecked()){
            memcpy((char*)(pdu->caMsg)+j*32,cblist[i]->text().toStdString().c_str(),cblist[i]->text().size());
            j++;
        }

    }
    memcpy((char*)(pdu->caMsg)+num*32,strPath.toStdString().c_str(),strPath.size());
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu = NULL;
}

void ShareFile::shareCancel()
{
    hide();
}


