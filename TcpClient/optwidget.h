#ifndef OPTWIDGET_H
#define OPTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "file.h"
#include "friend.h"
#include <QStackedWidget>
class OptWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OptWidget(QWidget *parent = nullptr);
    static OptWidget &getInstance();
    Friend* getFriend();
    File* getFile();
signals:
private:
    QListWidget *p_listW;

    File *m_pFile;
    Friend *m_pFriend;

    QStackedWidget *m_pSW;
};

#endif // OPTWIDGET_H
