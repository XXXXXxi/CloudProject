#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include "protocol.h"
#include <QFile>
#include <QTcpSocket>
#include <QTimer>
#include <windows.h>
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    // explicit MyTcpSocket(QObject *parent = nullptr);
    MyTcpSocket();
    QString getName();
    void copyDir(QString strSrDir,QString strDesDir);
public slots:
    void recvMsg();
    void ClientOffline();
    void DownloadFileData();
signals:
    void Offline(MyTcpSocket* mySocket);
private:
    QString strName;

    QFile m_file;
    qint64 m_iTotal;
    qint64 m_iRecved;
    bool m_bUpload;

    QTimer *m_pTimer;
    QString m_strDownloadPath;

};

#endif // MYTCPSOCKET_H
