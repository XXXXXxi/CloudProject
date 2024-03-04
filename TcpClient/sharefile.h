#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include "tcpclient.h"

class ShareFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShareFile(QWidget *parent = nullptr);
    static ShareFile& getInstance();
    void test();
    void updateFriend(QListWidget* pFriendList);
public slots:
    void cancelSelect();
    void selectAll();
    void shareSuccess();
    void shareCancel();
signals:
private:
    QPushButton *m_pSelectAllPB;
    QPushButton *m_pCancelSelectPB;

    QPushButton *m_pOKPB;
    QPushButton *m_pCancelPB;

    QScrollArea *m_pSA;
    QWidget *m_pFriendW;
    QVBoxLayout *m_pFriendVBL;
    QButtonGroup *m_pButtonGroup;
};

#endif // SHAREFILE_H
