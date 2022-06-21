#ifndef DATABASE_H
#define DATABASE_H

#include <QMap>
#include <QString>
#include <QObject>
#include <QSql>
#include <QSqlError>
#include <QSqlDatabase>
#include <types.h>
#include <QVariant>

#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>

#include <QDebug>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);

    User& find_or_create(const user_id id);

    QSqlError connect_db (QString &name);
    QSqlError disconnect_db();
    QVariant  sql_request(QString &request);

public:
    QString m_login_db;
    QString m_password_db;

signals:

private:
    QSqlDatabase db;
private slots:

private:
    QMap<user_id, User> m_users;
};

#endif // DATABASE_H
