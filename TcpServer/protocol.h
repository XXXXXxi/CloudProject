#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef unsigned int uint;

#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed : name existed"

#define LOGIN_OK "login ok"
#define LOGIN_FAILED "login failed : name or password error"

#define ALL_ONLINE_OK "all online ok"
#define ALL_ONLINE_FAILED "all online failed"

#define SEARCH_NO_EXIST "No such user"
#define SEARCH_ONLINE "user oniline"
#define SEARCH_OFFLINE "user offline"

#define ADD_ALREADY_FRIEND "Already friend"
// #define ADD_ONLINE "Add online"
#define ADD_OFFLINE "Add offline"
#define ADD_SUCCESS "Add success"
#define ADD_FAILED "Add failed"

#define CREATE_DIR_SUCCESS "create dir success"
#define CREATE_DIR_FAILED "create dir failed"
#define CREATE_DIR_EXIST "dir has existed"

#define FLUSH_DIR_SUCCESS "flush dir success"
#define FLUSH_DIR_FAILED "flush dir failed"

#define DELETE_DIR_SUCCESS "delete dir success"
#define DELETE_DIR_FAILED "delete dir failed"
#define DELETE_DIR_NOT_EXIST "delete dir : not exist"
#define DELETE_DIR_NOT_FILE "delete dir : isn't dir "

#define RENAME_DIR_SUCCESS "rename dir success"
#define RENAME_DIR_FAILED "rename dir failed"

#define BACK_DIR_SUCCESS "back dir success"
#define BACK_DIR_FAILED "back dir failed"

#define UPLOAD_FILE_SUCCESS "upload file success"
#define UPLOAD_FILE_FAILED "upload file failed"

#define DELETE_FILE_NOT_EXIST "delete file not exit"
#define DELETE_FILE_SUCCESS "delete file success"
#define DELETE_FILE_FAILED "delete file failed"
#define DELETE_FILE_NOT_FILE "delete file is dir"

#define DOWNLOAD_FILE_SUCCESS "download file success"
#define DOWNLOAD_FILE_FAILED "download file failed"

#define DOWNLOAD_FILE_DATA_SUCCESS "dowload file open success"
#define DOWNLOAD_FILE_DATA_FAILED "dowload file open failed"

enum ENUM_MSG_TYPE {
    ENUM_MSG_TYPE_MIN = 0,
    ENUM_MSG_TYPE_REGIST_REQUEST,   // 注册请求
    ENUM_MSG_TYPE_REGIST_RESPOND,   // 注册回复

    ENUM_MSG_TYPE_LOGIN_REQUEST,   // 登录请求
    ENUM_MSG_TYPE_LOGIN_RESPOND,   // 登录回复

    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,   // 全部在线请求
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,   // 全部在线回复

    ENUM_MSG_TYPE_SEARCH_USER_REQUEST,   // 搜索用户请求
    ENUM_MSG_TYPE_SEARCH_USER_RESPOND,   // 搜索用户回复

    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,   // 加好友请求
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,   // 加好友回复

    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST, // 刷新好友请求
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND, // 刷新好友回复

    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST, // 删除好友请求
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND, // 删除好友回复

    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST, // 私聊请求
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND, // 私聊回复

    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,   // 群聊请求
    ENUM_MSG_TYPE_GROUP_CHAT_RESPOND,   // 群聊回复

    ENUM_MSG_TYPE_CREATE_DIR_RESQUEST,  // 创建文件夹请求
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,   // 创建文件夹回复

    ENUM_MSG_TYPE_FLUSH_DIR_RESQUEST,  // 创建文件夹请求
    ENUM_MSG_TYPE_FLUSH_DIR_RESPOND,   // 创建文件夹回复

    ENUM_MSG_TYPE_DELETE_DIR_RESQUEST,  // 删除文件夹请求
    ENUM_MSG_TYPE_DELETE_DIR_RESPOND,   // 删除文件夹回复

    ENUM_MSG_TYPE_RENAME_DIR_RESQUEST,  // 重命名文件夹请求
    ENUM_MSG_TYPE_RENAME_DIR_RESPOND,   // 重命名文件夹回复

    ENUM_MSG_TYPE_BACK_DIR_RESQUEST,  // 返回上一层请求
    ENUM_MSG_TYPE_BACK_DIR_RESPOND,   // 返回上一层回复

    ENUM_MSG_TYPE_UPLOAD_FILE_RESQUEST, // 上传文件请求
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,  // 上传文件回复

    ENUM_MSG_TYPE_DELETE_FILE_REQUEST,  // 删除文件请求
    ENUM_MSG_TYPE_DELETE_FILE_RESPOND,  // 删除文件回复

    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST, // 下载文件请求
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND, // 下载文件回复

    ENUM_MSG_TYPE_DOWNLOAD_FILE_DATA,   // 传送下载文件数据

    ENUM_MSG_TYPE_SHARE_FILE_REQUEST, // 分享文件请求
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND, // 分享文件回复
    ENUM_MSG_TYPE_SHARE_FILE_NOTE,  //分享文件接受通知
    ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND,  //分享文件接受回复

    ENUM_MSG_TYPE_MAX = 0x00ffffff,
};

struct PDU
{
    PDU() {}
    uint uiPDULen;  //总的协议数据单元大小
    uint uiMsgType; //消息类型
    char caData[64];
    uint uiMsgLen;  //实际消息长度
    int caMsg[];    //实际消息
};

struct FileInfo{
    char fileName[64];
    bool isFile;
};

PDU *mkPDU(uint uMsglen);
#endif // PROTOCOL_H
