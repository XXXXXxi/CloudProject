#include "dboperator.h"
#include "mytcpserver.h"

#include <qmessagebox.h>

DBOperator::DBOperator(QObject *parent)
    : QObject{parent}
{
    // DBOperator::getInstance().init();
    init();

}

DBOperator &DBOperator::getInstance()
{
    static DBOperator instance;
    return instance;
}

void DBOperator::init()
{
    m_db = QSqlDatabase::addDatabase("QMYSQL");
    m_db.setHostName("localhost");
    m_db.setPort(3306);
    m_db.setUserName("root");
    m_db.setPassword("123456");
    m_db.setDatabaseName("cloud");
    if(m_db.open()){
        QSqlQuery query;
        query.exec("select * from userInfo");
        while(query.next()){
            QString data = QString("%1,%2,%3,%4")
                .arg(query.value(0).toString())
                .arg(query.value(1).toString())
                .arg(query.value(2).toString())
                .arg(query.value(3).toString());
            qDebug() << data;
        }
    }else{
        QMessageBox::critical(NULL,"打开数据库","打开数据库失败");
    }
}

DBOperator::~DBOperator()
{
    m_db.close();
}

bool DBOperator::hanleRegist(const char *name, const char *pwd)
{
    if(NULL == name || NULL == pwd){
        return false;
    }
    QString caQuery = QString("insert into userInfo(name,pwd) values(\"%1\",\"%2\")").arg(name).arg(pwd);
    qDebug() << caQuery;
    QSqlQuery query;
    return query.exec(caQuery);
}

bool DBOperator::hanleLogin(const char *name, const char *pwd)
{
    if(NULL == name || NULL == pwd){
        return false;
    }
    QString caQuery = QString("select * from userInfo where name = \"%1\" and pwd = \"%2\" and online = 0").arg(name).arg(pwd);
    // qDebug() << caQuery;
    QSqlQuery query;
    query.exec(caQuery);
    if(query.next()){
        caQuery = QString("update userInfo set online = 1 where name  = \"%1\" and pwd = \"%2\"").arg(name).arg(pwd);
        qDebug() << caQuery ;
        return query.exec(caQuery);
    }else{
        return false;
    }
    return true;
}

bool DBOperator::handleOffline(const char* name)
{
    if(NULL == name) {
        return false;
    }
    QString caQuery = QString("update userInfo set online = 0 where name = \"%1\"").arg(name);
    QSqlQuery query;
    return query.exec(caQuery);

}

QStringList DBOperator::handleAllOnline()
{
    QString caQuery = QString("select name from userInfo where online = 1");
    QSqlQuery query;
    query.exec(caQuery);
    QStringList result;
    result.clear();
    while(query.next()) {
        result.append(query.value(0).toString());
    }
    return result;

}

int DBOperator::hanleSearchUser(const char *name)
{
    QString caQuery = QString("select online from userInfo where name = \"%1\"").arg(name);
    QSqlQuery query;
    query.exec(caQuery);
    if(query.next()){
        return query.value(0).toInt();
    }else{
        return -1;
    }
}

int DBOperator::hanldeAddFriend(const char *requestName, const char *respondName)
{
    if(NULL == requestName || NULL == respondName){
        return -2;
    }
    QString caQuery = QString("select * from friend where (id = (select id from userInfo where name = \"%1\") and"
                              " friendId = (select id from userInfo where name =\"%2\"))"
                              " or ((id = (select id from userInfo where name = \"%3\") and"
                              " friendId = (select id from userInfo where name =\"%4\")))").arg(requestName).arg(respondName)
                          .arg(respondName).arg(requestName);
    qDebug() << caQuery;
    QSqlQuery query;
    query.exec(caQuery);
    if(query.next()){
        return -1;
    }else{
        caQuery = QString("select online from userInfo where name = \"%1\"").arg(respondName);
        QSqlQuery query;
        query.exec(caQuery);
        if(query.next()){
            return query.value(0).toInt();
        }
    }
    return 1;
}

bool DBOperator::hanldeAddFriendSuccess(const char *requestName, const char *respondName)
{
    if(NULL == requestName || NULL == respondName){
        return false;
    }
    QString caQuery = QString("insert friend(id,friendId) values((select id from userinfo where name = '%1'),"
                              "(select id from userinfo where name = '%2'))").arg(requestName).arg(respondName);
    QSqlQuery query;
    return query.exec(caQuery);

}

QStringList DBOperator::hanldeFlushFriend(const char *name)
{

    QStringList list;
    if(NULL == name){
        return list;
    }
    QString caQuery = QString("select name from userinfo where id in (select id from friend where friendId = "
                              "(select id from userinfo where name = '%1'))").arg(name);
    QSqlQuery query;
    query.exec(caQuery);
    // qDebug() << caQuery;
    list.clear();
    while(query.next()){
        // qDebug() <<"flush name :" <<query.value(0).toString();
        list.append(query.value(0).toString());
    }
    caQuery = QString("select name from userinfo where id in (select friendId from friend where id = "
                      "(select id from userinfo where name = '%1'))").arg(name);
    query.exec(caQuery);
    while(query.next()){
        // qDebug() << query.value(0).toString();
        list.append(query.value(0).toString());
    }
    return list;
}

bool DBOperator::handlerDeleteFriend(const char *requestName, const char *respondName)
{
    if(NULL == requestName || NULL == respondName){
        return false;
    }
    QString caQuery = QString("delete from friend where id = (select id from userinfo where name = '%1' ) and "
                              "friendId = (select id from userinfo where name = '%2')").arg(requestName).arg(respondName);
    QSqlQuery query;
    query.exec(caQuery);
    caQuery = QString("delete from friend where friendId = (select id from userinfo where name = '%1' ) and "
                      "id = (select id from userinfo where name = '%2')").arg(requestName).arg(respondName);
    query.exec(caQuery);
    return true;
}
