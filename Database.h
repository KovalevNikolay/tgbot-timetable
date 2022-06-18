#ifndef DATABASE_H
#define DATABASE_H

#include <QMap>
#include <QString>
#include <QObject>
#include <QSql>
#include <QSqlDatabase>
#include <types.h>

using user_id = qint32;

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QObject *parent = nullptr);

signals:

private:
    void connect_db();

private:
    QString m_login_db;
    QString m_password_db;

    QMap<user_id, User> m_users;
};

#endif // DATABASE_H
