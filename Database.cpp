#include "Database.h"

Database::Database(QObject *parent)
    : QObject{parent}
{
        db= QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("C:\\qt_projects\\tgbot.db");

    if (!db.open())
    {
      qDebug()<<db.lastError().text();
    }
    else
    {
      qDebug()<<"Success";
    }

}
User& Database::find_or_create(const user_id id)
{
    auto it = m_users.find(id);
    if(it != m_users.end())
    {
        return *it;
    } else
    {
        Telegram::User user(id);
        it = m_users.insert(id, user);
        qDebug() << "User created " << user;

        User user_db(user);
        // QtConcurence::run([&user_db](){
        //
        // TODO async write user data to database with .sql
        // NOTE dont forget mutex in writer method db if its need
        //  });

        return *it;
    }
}

QSqlError Database::connect_db(QString &name)
{
    db= QSqlDatabase::addDatabase("QPSQL");
    db.setDatabaseName(name);
}

QVariant Database::sql_request(QString &request)
{
    QSqlQuery query= QSqlQuery(db);
    if (!query.exec(request))
    {
        qDebug()<<query.lastError().databaseText();
        qDebug()<<query.lastError().driverText();
        return 0;
    }
    while (query.next())
    {
        qDebug() << query.record();
    }
}

