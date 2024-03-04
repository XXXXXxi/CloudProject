#include "optwidget.h"

OptWidget::OptWidget(QWidget *parent)
    : QWidget{parent}
{
    p_listW = new QListWidget(this);
    p_listW->addItem("好友");
    p_listW->addItem("文件");

    m_pFile = new File;
    m_pFriend = new Friend;

    m_pSW = new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pFile);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(p_listW);
    pMain->addWidget(m_pSW);

    setLayout(pMain);

    connect(p_listW,SIGNAL(currentRowChanged(int)),m_pSW,SLOT(setCurrentIndex(int)));
}

OptWidget &OptWidget::getInstance()
{
    static OptWidget instance;
    return instance;
}

Friend *OptWidget::getFriend()
{
    return m_pFriend;
}

File *OptWidget::getFile()
{
    return m_pFile;
}
