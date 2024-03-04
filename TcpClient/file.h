#ifndef FILE_H
#define FILE_H

#include "protocol.h"

#include <QListWidget>
#include <QPushButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTimer>

class File : public QWidget
{
    Q_OBJECT
public:
    explicit File(QWidget *parent = nullptr);
    void showCreateFileMsg(PDU *pdu);
    void showFlushList(PDU *pdu);
    void showDeleteDirMsg(PDU *pdu);
    void showRenameDirMsg(PDU *pdu);
    void showUploadFileMsg(PDU *pdu);
    void showDeleteFileMsg(PDU *pdu);
    QString getStrDownloadFile();
    void setStrDownloadFile(QString downloadFile);
    void recvDownlodFileInfo(PDU *pdu);
    void recvDownloadFileData(PDU *pdu);
    QString getStrShareFileName();
    void recvShareFileInfo(PDU *pdu);
public slots:
    void createDir();
    void flushFileList();
    void doubleDir(QListWidgetItem* pItem);
    void deleteDir();
    void renameDir();
    void backLastDir();
    void uploadFile();
    void uploadFileData();
    void deleteFile();
    void downloadFile();
    void shareFile();
signals:
    void flushFileListSignal();
private:
    QListWidget *m_pFileListW;
    QPushButton *m_pReturnPB;
    QPushButton *m_pCreateDirPB;
    QPushButton *m_pDelDirPB;
    QPushButton *m_pRenameDirPB;
    QPushButton *m_pFlushDirPB;
    QPushButton *m_pUploadFilePB;
    QPushButton *m_pDownloadFilePB;
    QPushButton *m_pDelFilePB;
    QPushButton *m_pShareFilePB;

    QString m_strUploadFileFile;
    QString m_strDownloadPath;
    QString m_strShareFileName;
    qint64 m_FileSize;
    qint64 m_resvSize;

    QTimer *m_pTimer;
};

#endif // FILE_H
