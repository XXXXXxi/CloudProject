#include "DBOperator.h"
#include "mytcpsocket.h"

#include <MyTcpServer.h>
#include <QDir>

// MyTcpSocket::MyTcpSocket(QObject *parent)
//     : QTcpSocket{parent}
// {}

MyTcpSocket::MyTcpSocket()
{
    m_pTimer = new QTimer;
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    connect(this,SIGNAL(disconnected()),this,SLOT(ClientOffline()));
    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(DownloadFileData()));

    m_bUpload = false;
}

QString MyTcpSocket::getName()
{
    return strName;
}

void MyTcpSocket::copyDir(QString strSrDir, QString strDesDir)
{
    QDir dir;
    dir.mkdir(strDesDir);

    dir.setPath(strSrDir);
    QString srcTmp;
    QString destTmp;
    QFileInfoList fileInfoList = dir.entryInfoList();
    for(int i = 0;i<fileInfoList.size();i++){
        qDebug() << fileInfoList[i].fileName();
        srcTmp = strSrDir + "/" + fileInfoList[i].fileName();
        destTmp = strDesDir + '/' + fileInfoList[i].fileName();
        if(fileInfoList[i].isFile()) {
            QFile::copy(srcTmp,destTmp);
        }else if(fileInfoList[i].isDir()){
            if(QString(".") == fileInfoList[i].fileName()
                || QString("..") == fileInfoList[i].fileName()) {
                continue;
            }
            copyDir(srcTmp,destTmp);
        }
    }
}

void MyTcpSocket::recvMsg()
{
    if(!m_bUpload) {
        // qDebug() << this->bytesAvailable();
        uint uiPDULen = 0;
        this->read((char*)&uiPDULen,sizeof(uint));
        uint uiMsgLen = uiPDULen-sizeof(PDU);
        if(uiMsgLen > 4096){
            uiMsgLen = 4096;
            uiPDULen = uiMsgLen + sizeof(PDU);
        }

        PDU *pdu = mkPDU(uiMsgLen);
        this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
        // qDebug() << pdu->uiMsgType << " " << (char*)(pdu->caMsg);
        switch(pdu->uiMsgType){
        case ENUM_MSG_TYPE_REGIST_REQUEST:{
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            qDebug() << caName << " " << caPwd << " " << pdu->uiMsgType;
            bool ret = DBOperator::getInstance().hanleRegist(caName,caPwd);
            QDir dir;
            qDebug() << "create dir : " << dir.mkdir(QString("./%1").arg(caName));
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_REGIST_RESPOND;
            if(ret){
                strcpy(respdu->caData,REGIST_OK);
            }else{
                strcpy(respdu->caData,REGIST_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_LOGIN_REQUEST:{
            char caName[32] = {'\0'};
            char caPwd[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(caPwd,pdu->caData+32,32);
            qDebug() << caName << " " << caPwd << " " << pdu->uiMsgType;
            bool ret = DBOperator::getInstance().hanleLogin(caName,caPwd);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_LOGIN_RESPOND;
            if(ret){
                strcpy(respdu->caData,LOGIN_OK);
                strName = caName;
            }else{
                strcpy(respdu->caData,LOGIN_FAILED);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:{
            QStringList ret = DBOperator::getInstance().handleAllOnline();
            uint uiMsgLen = ret.size()*32;
            PDU *respdu = mkPDU(uiMsgLen);
            respdu->uiMsgType = ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
            for(int i = 0;i<ret.size();i++){
                memcpy((char*)(respdu->caMsg)+32*i,
                       ret.at(i).toStdString().c_str(),
                       ret.at(i).size());
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USER_REQUEST:{
            char caName[32] = {'\0'};
            // qDebug() << "name : " << pdu->caData;
            strncpy(caName,pdu->caData,32);
            // qDebug() << caName ;
            int ret = DBOperator::getInstance().hanleSearchUser(caName);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SEARCH_USER_RESPOND;
            if(ret == 1) {
                strncpy(respdu->caData,SEARCH_ONLINE,32);
            }else if(ret == 0){
                strncpy(respdu->caData,SEARCH_OFFLINE,32);
            }else{
                strncpy(respdu->caData,SEARCH_NO_EXIST,32);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{
            char caName[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            qDebug() << caName ;
            int ret = DBOperator::getInstance().hanldeAddFriend(strName.toStdString().c_str(),caName);
            qDebug() << "Add friend sql result " << ret ;
            strncpy(pdu->caData,strName.toStdString().c_str(),32);
            if(ret == -1) {
                PDU *respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strncpy(respdu->caData,ADD_ALREADY_FRIEND,32);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }else if(ret == 0){
                PDU *respdu = mkPDU(0);
                respdu->uiMsgType = ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
                strncpy(respdu->caData,ADD_OFFLINE,32);
                write((char*)respdu,respdu->uiPDULen);
                free(respdu);
                respdu = NULL;
            }else{
                MyTcpSocket* myTcpSocket = MyTcpServer::getInstance().getMyTcpSocket(caName);
                strncpy(pdu->caData,strName.toStdString().c_str(),32);
                myTcpSocket->write((char*)pdu,pdu->uiPDULen);
                free(pdu);
                pdu = NULL;
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:{
            char caName[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            if(0 != strcmp(pdu->caData,ADD_FAILED)) {
                DBOperator::getInstance().hanldeAddFriendSuccess(caName,strName.toStdString().c_str());
            }
            MyTcpServer::getInstance().getMyTcpSocket(caName)->write((char*)pdu,pdu->uiPDULen);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:{
            QStringList ret = DBOperator::getInstance().hanldeFlushFriend(strName.toStdString().c_str());
            PDU *respdu = mkPDU(ret.size()*32);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;
            for(int i = 0;i<ret.size();i++){
                strncpy((char*)(respdu->caMsg)+32*i,ret.value(i).toStdString().c_str(),32);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST :{
            char caName[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            DBOperator::getInstance().handlerDeleteFriend(strName.toStdString().c_str(),caName);
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
            write((char*)respdu,respdu->uiPDULen);
            strncpy(pdu->caData,strName.toStdString().c_str(),32);
            MyTcpServer::getInstance().getMyTcpSocket(caName)->write((char*)pdu,pdu->uiPDULen);
            free(respdu);
            free(pdu);
            respdu=NULL;
            pdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:{
            char caName[32] = {'\0'};
            strncpy(caName,pdu->caData,32);
            strncpy(pdu->caData,strName.toStdString().c_str(),32);
            MyTcpServer::getInstance().getMyTcpSocket(caName)->write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:{
            strncpy(pdu->caData,strName.toStdString().c_str(),32);
            MyTcpServer::getInstance().sendAllUser(pdu);
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_RESQUEST:{
            QDir dir;
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
            if(dir.exists((char*)pdu->caMsg)){
                strncpy(respdu->caData,CREATE_DIR_EXIST,64);
            }else{
                if(dir.mkdir((char*)pdu->caMsg)){
                    strncpy(respdu->caData,CREATE_DIR_SUCCESS,64);
                }else{
                    strncpy(respdu->caData,CREATE_DIR_FAILED,64);
                }
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_DIR_RESQUEST:{
            char caPath[32] = {'\0'};
            strncpy(caPath,pdu->caData+32,32);
            QDir existDir;
            if(!existDir.exists(caPath)){
                pdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_RESPOND;
                strncpy(pdu->caData,FLUSH_DIR_FAILED,32);
                write((char*)pdu,pdu->uiPDULen);
                break;
            }
            QDir dir(caPath);
            QFileInfoList list = dir.entryInfoList();
            PDU *respdu = mkPDU(list.size()*sizeof(FileInfo));
            strncpy(respdu->caData,FLUSH_DIR_SUCCESS,32);
            strncpy(respdu->caData+32,pdu->caData+32,32);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_RESPOND;
            FileInfo *fileInfo = NULL;
            for(int i = 0;i<list.size();i++){
                qDebug() << list[i].fileName() << " " << list[i].isFile();
                fileInfo = (FileInfo*)(respdu->caMsg)+i;
                strncpy(fileInfo->fileName ,list[i].fileName().toStdString().c_str(),64);
                fileInfo->isFile = list[i].isFile();
            }
            write((char*)(respdu),respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_DIR_RESQUEST: {
            char *caName = new char(pdu->uiMsgLen+1);
            strncpy(caName,(char*)pdu->caMsg,pdu->uiMsgLen);
            QDir dir;
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_DIR_RESPOND;
            if(!dir.exists(caName)){
                strncpy(respdu->caData,DELETE_DIR_NOT_EXIST,64);
            }else {
                QFileInfo fileInfo(caName);
                if(fileInfo.isDir()) {
                    dir.setPath(caName);
                    if(dir.removeRecursively()) {
                        strncpy(respdu->caData,DELETE_DIR_SUCCESS,64);
                    }else{
                        strncpy(respdu->caData,DELETE_DIR_FAILED,64);
                    }
                }else{
                    strncpy(respdu->caData,DELETE_DIR_NOT_FILE,64);
                }
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_RENAME_DIR_RESQUEST: {
            char oldName[32] = {'\0'};
            char newName[32] = {'\0'};
            char *currPath = new char(pdu->uiMsgLen);
            strncpy(oldName,pdu->caData,32);
            strncpy(newName,pdu->caData+32,32);
            strncpy(currPath,(char*)pdu->caMsg,pdu->uiMsgLen);
            QDir dir;
            int ret = dir.rename(QString("%1/%2").arg(currPath).arg(oldName),QString("%1/%2").arg(currPath).arg(newName));
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_RENAME_DIR_RESPOND;
            if(ret){
                strncpy(respdu->caData,RENAME_DIR_SUCCESS,32);
            }else{
                strncpy(respdu->caData,RENAME_DIR_FAILED,32);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_BACK_DIR_RESQUEST:{
            char *curPath = new char(pdu->uiMsgLen+1);
            strncpy(curPath,(char*)pdu->caMsg,pdu->uiMsgLen);
            qDebug() << curPath ;
            QDir dir(curPath);
            QFileInfoList list = dir.entryInfoList();
            PDU *respdu = mkPDU(list.size()*sizeof(FileInfo));
            strncpy(respdu->caData,FLUSH_DIR_SUCCESS,32);
            strncpy(respdu->caData+32,curPath,32);
            respdu->uiMsgType = ENUM_MSG_TYPE_FLUSH_DIR_RESPOND;
            FileInfo *fileInfo = NULL;
            for(int i = 0;i<list.size();i++){
                qDebug() << list[i].fileName() << " " << list[i].isFile();
                fileInfo = (FileInfo*)(respdu->caMsg)+i;
                strncpy(fileInfo->fileName ,list[i].fileName().toStdString().c_str(),64);
                fileInfo->isFile = list[i].isFile();
            }
            write((char*)(respdu),respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_RESQUEST:{
            qint64 size = 0;
            char *fileName = new char[pdu->uiMsgLen+1];
            sscanf(pdu->caData,"%lld",&size);
            strncpy(fileName,(char*)pdu->caMsg,pdu->uiMsgLen);
            qDebug() << fileName << " " << size;
            m_file.setFileName(fileName);
            if(m_file.open(QIODevice::WriteOnly)) {
                m_bUpload = true;
                m_iTotal = size;
                m_iRecved = 0;
            }
            delete []fileName;
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FILE_REQUEST:{
            char *caName = new char(pdu->uiMsgLen+1);
            strncpy(caName,(char*)pdu->caMsg,pdu->uiMsgLen);
            qDebug() << caName;
            QDir dir;
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DELETE_FILE_RESPOND;
            if(!dir.exists(caName)){
                strncpy(respdu->caData,DELETE_FILE_NOT_EXIST,64);
            }else {
                QFileInfo fileInfo(caName);
                if(fileInfo.isFile()) {
                    if(dir.remove(caName)) {
                        strncpy(respdu->caData,DELETE_FILE_SUCCESS,64);
                    }else{
                        strncpy(respdu->caData,DELETE_FILE_FAILED,64);
                    }
                }else{
                    strncpy(respdu->caData,DELETE_FILE_NOT_FILE,64);
                }
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:{
            char *caName = new char(pdu->uiMsgLen+1);
            strncpy(caName,(char*)pdu->caMsg,pdu->uiMsgLen);
            m_strDownloadPath = QString(caName);
            qDebug() << caName;
            PDU *respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
            QFileInfo fileInfo(caName);
            if(fileInfo.isFile()) {
                strncpy(respdu->caData,DOWNLOAD_FILE_SUCCESS,32);
                sprintf(respdu->caData+32,"%lld",fileInfo.size());
            }else{
                strncpy(respdu->caData,DOWNLOAD_FILE_FAILED,32);
            }
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;

            m_pTimer->start(1000);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:{
            char caSendName[32] = {'\0'};
            int num = 0;
            sscanf(pdu->caData,"%s%d",caSendName,&num);
            int size = num*32;
            PDU* respdu = mkPDU(pdu->uiMsgLen-size);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE;
            strcpy(respdu->caData,caSendName);
            memcpy((char*)(respdu->caMsg),(char*)(pdu->caMsg)+size,pdu->uiMsgLen-size);

            char caRecnName[32] = {'\0'};
            for(int i = 0;i<num;i++) {
                memcpy(caRecnName,(char*)(pdu->caMsg)+i*32,32);
                MyTcpServer::getInstance().getMyTcpSocket(caRecnName)->write((char*)respdu,respdu->uiPDULen);
            }

            free(respdu);
            respdu = NULL;

            respdu = mkPDU(0);
            respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
            strcpy(respdu->caData,"share file ok");
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND:{
            QString strRecvPath = QString("./%1").arg(pdu->caData);
            QString strSharePath = QString("%1").arg((char*)(pdu->caMsg));
            int index = strSharePath.lastIndexOf('/');
            QString strFileName = strSharePath.right(strSharePath.size()-index-1);
            strRecvPath = strRecvPath + '/' + strFileName;
            QFileInfo fileInfo(strSharePath);
            if(fileInfo.isFile()){
                QFile::copy(strSharePath,strRecvPath);
            }else if(fileInfo.isDir()){
                copyDir(strSharePath,strRecvPath);
            }
            break;
        }
        default:
            break;
        }
        free(pdu);
        pdu = NULL;
    }else{
        QByteArray buff = readAll();
        m_file.write(buff);
        m_iRecved += buff.size();
        PDU *respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
        qDebug() << m_iTotal << " " << m_iRecved;
        if(m_iTotal == m_iRecved) {
            m_file.close();
            m_bUpload = false;
            strncpy(respdu->caData,UPLOAD_FILE_SUCCESS,32);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }else if(m_iTotal < m_iRecved){
            m_file.close();
            m_bUpload = false;
            strncpy(respdu->caData,UPLOAD_FILE_FAILED,32);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu = NULL;
        }

    }

}

void MyTcpSocket::ClientOffline()
{
    if(DBOperator::getInstance().handleOffline(strName.toStdString().c_str())){
        emit Offline(this);
    }else{
        qDebug() << "offline failed : " << strName ;
    }
}

void MyTcpSocket::DownloadFileData()
{
    m_pTimer->stop();
    QFile file(m_strDownloadPath);
    char *pBuffer = new char[4096];
    if(!file.open(QIODevice::ReadOnly)) {
        // QMessageBox::warning(this,"上传文件","打开文件失败");
        return ;
    }
    qint64 ret = 0;
    while(true) {
        ret = file.read(pBuffer,4096);
        if(ret>0 && ret <= 4096) {
            qDebug() << "upload size :" << ret;
            write(pBuffer,ret);
        }else if(0 == ret) {
            break;
        }
    }
    file.close();
    delete []pBuffer;

}
