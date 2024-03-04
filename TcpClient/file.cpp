#include "file.h"
#include "optwidget.h"
#include "sharefile.h"
#include "tcpclient.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

File::File(QWidget *parent)
    : QWidget{parent}
{
    m_pTimer = new QTimer;

    m_pFileListW = new QListWidget;
    m_pReturnPB = new QPushButton("返回");
    m_pCreateDirPB = new QPushButton("创建文件夹");
    m_pDelDirPB = new QPushButton("删除文件夹");
    m_pRenameDirPB = new QPushButton("重命名文件夹");
    m_pFlushDirPB = new QPushButton("刷新文件夹");

    QVBoxLayout *pDirVBL = new QVBoxLayout;
    pDirVBL->addWidget(m_pReturnPB);
    pDirVBL->addWidget(m_pCreateDirPB);
    pDirVBL->addWidget(m_pDelDirPB);
    pDirVBL->addWidget(m_pRenameDirPB);
    pDirVBL->addWidget(m_pFlushDirPB);

    m_pUploadFilePB = new QPushButton("上传文件");
    m_pDownloadFilePB = new QPushButton("下载文件");
    m_pDelFilePB = new QPushButton("删除文件");
    m_pShareFilePB = new QPushButton("分享文件");

    QVBoxLayout *pFileVBL = new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadFilePB);
    pFileVBL->addWidget(m_pDownloadFilePB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareFilePB);

    QHBoxLayout *pMain = new QHBoxLayout;
    pMain->addWidget(m_pFileListW);
    pMain->addLayout(pDirVBL);
    pMain->addLayout(pFileVBL);

    setLayout(pMain);


    connect(m_pCreateDirPB,SIGNAL(clicked(bool)),this,SLOT(createDir()));
    connect(m_pFlushDirPB,SIGNAL(clicked(bool)),this,SLOT(flushFileList()));
    connect(m_pFileListW,SIGNAL(itemDoubleClicked(QListWidgetItem*)),this,SLOT(doubleDir(QListWidgetItem*)));
    connect(m_pDelDirPB,SIGNAL(clicked(bool)),this,SLOT(deleteDir()));
    connect(m_pRenameDirPB,SIGNAL(clicked(bool)),this,SLOT(renameDir()));
    connect(this,SIGNAL(flushFileListSignal()),this,SLOT(flushFileList()));
    connect(m_pReturnPB,SIGNAL(clicked(bool)),this,SLOT(backLastDir()));
    connect(m_pUploadFilePB,SIGNAL(clicked(bool)),this,SLOT(uploadFile()));
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(uploadFileData()));
    connect(m_pDelFilePB,SIGNAL(clicked(bool)),this,SLOT(deleteFile()));
    connect(m_pDownloadFilePB,SIGNAL(clicked(bool)),this,SLOT(downloadFile()));
    connect(m_pShareFilePB,SIGNAL(clicked(bool)),this,SLOT(shareFile()));

    emit flushFileListSignal();
}


void File::showCreateFileMsg(PDU *pdu)
{
    if(pdu == NULL){
        return ;
    }
    if(0 == strcmp(pdu->caData,CREATE_DIR_SUCCESS)) {
        QMessageBox::information(this,"增加文件夹","增加文件夹成功");
    }else if(0 == strcmp(pdu->caData,CREATE_DIR_FAILED)) {
        QMessageBox::warning(this,"增加文件夹","增加文件夹失败");
    }else if(0 == strcmp(pdu->caData,CREATE_DIR_EXIST)) {
        QMessageBox::warning(this,"增加文件夹","已存在文件夹");
    }
    emit flushFileListSignal();
}

void File::showFlushList(PDU *pdu)
{
    if(pdu == NULL) {
        return ;
    }
    char res[32] = {'\0'};
    strncpy(res,pdu->caData,32);
    qDebug() << res;
    if(0 == strcmp(res,FLUSH_DIR_FAILED)){
        QMessageBox::warning(this,"刷新文件夹","无此地址");
        return ;
    }
    m_pFileListW->clear();
    int size = pdu->uiMsgLen/sizeof(FileInfo);
    FileInfo *fileInfo = NULL;
    for(int i = 2; i<size;i++){
        fileInfo = (FileInfo*)pdu->caMsg + i;
        qDebug() << fileInfo->fileName << " " << fileInfo->isFile;
        QListWidgetItem *pItem = new QListWidgetItem;
        if(fileInfo->isFile) {
            pItem->setIcon(QIcon(QPixmap(":/icon/file.jpg")));
        }else{
            pItem->setIcon(QIcon(QPixmap(":/icon/dir.jpeg")));
        }
        pItem->setText(fileInfo->fileName);
        m_pFileListW->addItem(pItem);
    }
    char caCurPath[32] = {'\0'};
    strncpy(caCurPath,pdu->caData+32,32);
    TcpClient::getInstance().setCurrPath(caCurPath);
}

void File::showDeleteDirMsg(PDU *pdu)
{
    if(NULL == pdu) {
        return ;
    }
    QMessageBox::information(this,"删除文件夹",pdu->caData);
    emit flushFileListSignal();
}

void File::showRenameDirMsg(PDU *pdu)
{
    if(NULL == pdu) {
        return ;
    }
    QMessageBox::information(this,"重命名文件夹",pdu->caData);
    emit flushFileListSignal();
}

void File::showUploadFileMsg(PDU *pdu)
{
    if(NULL == pdu) {
        return ;
    }
    QMessageBox::information(this,"重命名文件夹",pdu->caData);
    emit flushFileListSignal();
}

void File::showDeleteFileMsg(PDU *pdu)
{
    if(NULL == pdu) {
        return ;
    }
    QMessageBox::information(this,"删除文件夹",pdu->caData);
    emit flushFileListSignal();
}

QString File::getStrDownloadFile()
{
    return m_strDownloadPath;
}

void File::setStrDownloadFile(QString downloadFile)
{
    m_strDownloadPath = downloadFile;
}

void File::recvDownlodFileInfo(PDU *pdu)
{
    qDebug() << "download file info";
    if(NULL == pdu){
        return ;
    }
    if(0 == strncmp(pdu->caData,DOWNLOAD_FILE_FAILED,32)){
        QMessageBox::warning(this,"下载文件","下载文件失败");
    }else{
        sscanf(pdu->caData+32,"%lld",&m_FileSize);
        qDebug() << m_strDownloadPath << " " << m_FileSize;

        TcpClient::getInstance().setmFile(m_strDownloadPath);
        TcpClient::getInstance().setiTotal(m_FileSize);
        TcpClient::getInstance().setiRecved(0);
        TcpClient::getInstance().setbDownload(true);

        m_resvSize = 0;
    }
}

void File::recvDownloadFileData(PDU *pdu)
{
    if(NULL == pdu) {
        return ;
    }
    QFile file(m_strDownloadPath);
    if(file.open(QIODevice::ReadOnly | QIODevice::Append)) {
        quint64 size = 0;
        if(0 == strncmp(pdu->caData,DOWNLOAD_FILE_FAILED,32)) {
            QMessageBox::warning(this,"下载文件","下载文件失败");
        }else {
            sscanf(pdu->caData+32,"%lld",&size);
            m_resvSize+=size;
            char *data = new char[pdu->uiMsgLen];
            strncpy(data,(char*)pdu->caMsg,pdu->uiMsgLen);
            file.write(data);
            qDebug() << m_resvSize << " " << m_FileSize;
            if(m_resvSize == m_FileSize) {
                QMessageBox::information(this,"下载文件","下载文件成功");
            }
        }
    }else{
        QMessageBox::warning(this,"下载文件","下载文件失败");
    }
    file.close();

}

QString File::getStrShareFileName()
{
    return m_strShareFileName;
}

void File::recvShareFileInfo(PDU *pdu)
{
    if(NULL == pdu) {
        return ;
    }
    QMessageBox::information(this,"共享文件",pdu->caData);
    emit flushFileListSignal();
}

void File::createDir()
{
    QString newDirName = QInputDialog::getText(this,"新增文件夹","请输入新增文件夹");
    if(!newDirName.isEmpty()){
        QString Path = TcpClient::getInstance().getCurrPath() + QString("/%1").arg(newDirName);
        PDU *pdu = mkPDU(Path.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESQUEST;
        strncpy(pdu->caData,TcpClient::getInstance().getStrName().toStdString().c_str(),32);
        strncpy((char*)pdu->caMsg,Path.toStdString().c_str(),Path.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::warning(this,"新建文件夹","输入不能为0");
    }

}

void File::flushFileList()
{
    PDU *pdu = mkPDU(0);
    pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_RESQUEST;
    strncpy(pdu->caData+32,TcpClient::getInstance().getCurrPath().toStdString().c_str(),32);
    TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void File::doubleDir(QListWidgetItem* pItem)
{
    if(!pItem->text().isEmpty()) {
        QString curr = TcpClient::getInstance().getCurrPath() + QString("/%1").arg(pItem->text());
        qDebug() << TcpClient::getInstance().getCurrPath() << TcpClient::getInstance().getStrName() << curr ;
        PDU *pdu = mkPDU(0);
        pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_RESQUEST;
        strncpy(pdu->caData+32,curr.toStdString().c_str(),32);
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void File::deleteDir()
{
    if(NULL != m_pFileListW->currentItem()){
        QString name = m_pFileListW->currentItem()->text();
        QString curr = TcpClient::getInstance().getCurrPath() + QString("/") + name;
        qDebug()<< curr;
        PDU *pdu = mkPDU(curr.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_DIR_RESQUEST;
        strncpy((char*)pdu->caMsg,curr.toStdString().c_str(),curr.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void File::renameDir()
{
    if(NULL != m_pFileListW->currentItem()){
        QString oldName = m_pFileListW->currentItem()->text();
        QString curr = TcpClient::getInstance().getCurrPath();
        qDebug()<< curr;
        QString newName = QInputDialog::getText(this,"重命名文件夹","请输入新文件名");
        if(!newName.isEmpty()) {
            PDU *pdu = mkPDU(curr.size()+1);
            pdu->uiMsgType = ENUM_MSG_TYPE_RENAME_DIR_RESQUEST;
            strncpy(pdu->caData,oldName.toStdString().c_str(),32);
            strncpy(pdu->caData+32,newName.toStdString().c_str(),32);
            strncpy((char*)pdu->caMsg,curr.toStdString().c_str(),curr.size());
            TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
        }
    }
}

void File::backLastDir()
{
    if(TcpClient::getInstance().getCurrPath() != QString("./%1").arg(TcpClient::getInstance().getStrName())){
        QString currPath = TcpClient::getInstance().getCurrPath();
        int index = currPath.lastIndexOf('/');
        PDU *pdu = mkPDU(index+1);
        strncpy((char*)pdu->caMsg,currPath.toStdString().c_str(),index);
        qDebug() << (char*)pdu->caMsg;
        pdu->uiMsgType = ENUM_MSG_TYPE_BACK_DIR_RESQUEST;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }else{
        QMessageBox::warning(this,"返回上一级","已经位于顶层目录");
    }
}

void File::uploadFile()
{
    QString fileName = QFileDialog::getOpenFileName();
    QString currPath = TcpClient::getInstance().getCurrPath();
    m_strUploadFileFile = fileName;
    if(!fileName.isEmpty()) {
        int index = fileName.lastIndexOf('/');
        QString filePath =currPath + fileName.right(fileName.size()-index);
        // qDebug() << filePath;
        QFile file(fileName);
        qint64 fileSize = file.size();
        PDU *pdu = mkPDU(filePath.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESQUEST;
        strncpy((char*)pdu->caMsg,filePath.toStdString().c_str(),filePath.size());
        sprintf(pdu->caData,"%lld",fileSize);
        qDebug() << pdu->caMsg;
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;

        m_pTimer->start(1000);
    }else{
        QMessageBox::warning(this,"上传文件","选择文件为空");
    }

}

void File::uploadFileData()
{
    m_pTimer->stop();
    QFile file(m_strUploadFileFile);
    qDebug() << m_strUploadFileFile;
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this,"上传文件","打开文件失败");
        return ;
    }

    char *pBuffer = new char[4096];
    qint64 ret = 0;
    while(true) {
        ret = file.read(pBuffer,4096);
        if(ret>0 && ret <= 4096) {
            qDebug() << "upload size :" << ret;
            TcpClient::getInstance().getTcpSocket().write(pBuffer,ret);
        }else if(0 == ret) {
            break;
        }else{
            QMessageBox::warning(this,"上传文件","上传文件失败");
            break;
        }
    }
    file.close();
    delete []pBuffer;
}

void File::deleteFile()
{
    if(NULL != m_pFileListW->currentItem()){
        QString name = m_pFileListW->currentItem()->text();
        QString curr = TcpClient::getInstance().getCurrPath() + QString("/") + name;
        qDebug()<< curr;
        PDU *pdu = mkPDU(curr.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_REQUEST;
        strncpy((char*)pdu->caMsg,curr.toStdString().c_str(),curr.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;
    }
}

void File::downloadFile()
{
    if(NULL != m_pFileListW->currentItem()){
        QString name = m_pFileListW->currentItem()->text();
        QString curr = TcpClient::getInstance().getCurrPath() + QString("/") + name;
        // qDebug()<< curr;

        m_strDownloadPath = QFileDialog::getSaveFileName(this,"下载文件",name);
        qDebug() << curr << " " << m_strDownloadPath;

        PDU *pdu = mkPDU(curr.size()+1);
        pdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        strncpy((char*)pdu->caMsg,curr.toStdString().c_str(),curr.size());
        TcpClient::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu = NULL;


    }else{
        QMessageBox::warning(this,"下载文件","选择文件不能为空");
    }
}

void File::shareFile()
{

    if(NULL != m_pFileListW->currentItem()){
        m_strShareFileName = m_pFileListW->currentItem()->text();
        Friend *pFriend = OptWidget::getInstance().getFriend();
        QListWidget *pFriendList = pFriend->getFriendList();
        ShareFile::getInstance().updateFriend(pFriendList);

        if(ShareFile::getInstance().isHidden()) {
            ShareFile::getInstance().show();
        }
    }else{
        QMessageBox::warning(this,"共享文件","共享文件不能为空");
    }
}
