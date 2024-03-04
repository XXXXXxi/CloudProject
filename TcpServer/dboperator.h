#ifndef DBOPERATOR_H
#define DBOPERATOR_H

#include <QObject>
#include <QtSql/qsql.h>
#include <QtSql/qsqldatabase.h>
#include <QtSql/qsqlquery.h>
#include <QStringList>

class DBOperator : public QObject
{
    Q_OBJECT
public:
    static DBOperator& getInstance();
    ~DBOperator();
    explicit DBOperator(QObject *parent = nullptr);
    bool hanleRegist(const char *name,const char *pwd);
    bool hanleLogin(const char *name,const char *pwd);
    bool handleOffline(const char* name);
    QStringList handleAllOnline();
    int hanleSearchUser(const char* name);
    int hanldeAddFriend(const char* requestName,const char* respondName);
    bool hanldeAddFriendSuccess(const char* requestName,const char* respondName);
    QStringList hanldeFlushFriend(const char* name);
    bool handlerDeleteFriend(const char* requestName,const char* respondName);
private:
    QSqlDatabase m_db;
    void init();
signals:
};

#endif // DBOPERATOR_H
