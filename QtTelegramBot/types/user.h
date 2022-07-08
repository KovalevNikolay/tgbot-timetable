#ifndef USER_H
#define USER_H

#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QDataStream>

namespace Telegram {

class User
{
public:
    User() : id(0), firstname(QString()), lastname(QString()), username(QString()) {}
    User(QJsonObject user);
    User(const quint64 id) : id(id), firstname(QString()), lastname(QString()), username(QString()) {}

    quint64 id;
    QString firstname;
    QString lastname;
    QString username;
};

inline QDebug operator<< (QDebug dbg, const User &user)
{
    dbg.nospace() << qUtf8Printable(QString("Telegram::User(id=%1; firstname=%2; lastname=%3; username=%4)")
                     .arg(user.id)
                     .arg(user.firstname)
                     .arg(user.lastname)
                     .arg(user.username));
    return dbg.maybeSpace();
}
inline QDataStream &operator << (QDataStream &out, const User &user)
{
    out                  <<
           user.id       <<
           user.lastname <<
           user.lastname <<
           user.username;
    return out;
}
inline QDataStream &operator >> (QDataStream &in, User &user)
{
    in                   >>
           user.id       >>
           user.lastname >>
           user.lastname >>
           user.username;
    return in;
}
}

#endif // USER_H
