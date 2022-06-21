#ifndef DATABASE_H
#define DATABASE_H

#include <QString>
#include <QObject>
#include <QDebug>
#include <types.h>

#include <QDir>
#include <QSql>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>

#include <QMutex>
#include <QMutexLocker>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);

    QSqlError         connect_db(const QString &name);
    void              disconnect_db();
    QList<QSqlRecord> sql_request(const QString &request);

public:
    QString m_login_db;
    QString m_password_db;

private:
    QMutex       m_mtx;
    QSqlDatabase db;
};

#endif // DATABASE_H
