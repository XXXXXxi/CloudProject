#ifndef FRIEND_H
#define FRIEND_H

#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include "online.h"
#include "protocol.h"
#include "privatechat.h"

class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void showAllOnlineUser(PDU *pdu);
    void addFriendRequest(PDU *pdu);
    void addFriendRespond(PDU* pdu);
    void showFlushFriend(PDU *pdu);
    void showDeleteInfo(PDU *pdu);
    void showGroupCharMsg(PDU *pdu);
    QListWidget* getFriendList();
public slots:
    void showOnline();
    void searchUser();
    void flushFriend();
    void deleteFriend();
    void privateChat();
    void groupChat();
signals:
private:
    QTextEdit *m_pShowMsgTe;
    QListWidget *m_pFrinendListWidget;
    QLineEdit *m_pInputMsgLE;

    QPushButton *m_pDelFriendPB;
    QPushButton *m_pFlushFriendPB;
    QPushButton *m_pShowOnlineUserPB;
    QPushButton *m_pSearchUserPB;

    QPushButton *m_pMsgSendPB;
    QPushButton *m_pPrivateChatPB;

    Online *m_pOnline;
};

#endif // FRIEND_H
