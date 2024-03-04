#ifndef PRIVATECHAT_H
#define PRIVATECHAT_H

#include "protocol.h"

#include <QWidget>

namespace Ui {
class PrivateChat;
}

class PrivateChat : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChat(QWidget *parent = nullptr);
    ~PrivateChat();
    static PrivateChat& getInstance();
    void setSendToName(QString name);
    void showMsg(PDU *pdu);
private slots:
    void on_pushButton_clicked();

private:
    Ui::PrivateChat *ui;
    QString sendToName;
};

#endif // PRIVATECHAT_H
