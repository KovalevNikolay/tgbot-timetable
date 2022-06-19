#include "Database.h"

Database::Database(QObject *parent)
    : QObject{parent}
{

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
