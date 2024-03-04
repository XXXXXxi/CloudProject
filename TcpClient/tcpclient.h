#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include "protocol.h"
QT_BEGIN_NAMESPACE
namespace Ui {
class TcpClient;
}
QT_END_NAMESPACE

class TcpClient : public QWidget
{
    Q_OBJECT
public slots:
    void showConnect();
    void recvMsg();
public:
    TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig();
    static TcpClient& getInstance();
    QTcpSocket &getTcpSocket();
    QString getStrName();
    QString getCurrPath();
    QString setCurrPath(QString name);
    void setbDownload(bool status);
    void setiTotal(quint64 size);
    void setmFile(QString path);
    void setiRecved(quint64 size);

private slots:
    // void on_send_pb_clicked();

    void on_login_pb_clicked();
    void on_register_pb_clicked();
    void on_cancel_pb_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;
    quint16 m_usPort;
    QString strName;
    QString currPath;

    //连接服务器，和服务器数据交互
    QTcpSocket m_tcpSocket;

    quint64 m_iTotal;
    quint64 m_iRecved;
    QFile m_file;
    bool m_bDownload;
};
#endif // TCPCLIENT_H
