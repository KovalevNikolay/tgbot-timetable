#ifndef CHAT_H
#define CHAT_H

#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QDataStream>

namespace Telegram {

class Chat
{
public:
    Chat() {}
    Chat(QJsonObject chat);

    enum ChatType : quint8
    {
        Private,
        Group,
        Channel
    };

    qint32   id;
    ChatType type;
    QString  title;
    QString  username;
    QString  firstname;
    QString  lastname;
};

inline QDebug operator<< (QDebug dbg, const Chat &chat)
{
    dbg.nospace() << qUtf8Printable(QString("Telegram::Chat(id=%1; type=%2; title=%3; username=%4; firstname=%5; lastname=%6)")
                                    .arg(chat.id)
                                    .arg(chat.type)
                                    .arg(chat.title)
                                    .arg(chat.username)
                                    .arg(chat.firstname)
                                    .arg(chat.lastname));
    return dbg.maybeSpace();
}
inline QDataStream &operator << (QDataStream &out, const Chat &chat)
{
    out <<
           chat.id        <<
           chat.type      <<
           chat.title     <<
           chat.username  <<
           chat.firstname <<
           chat.lastname;
    return out;
}
inline QDataStream &operator >> (QDataStream &in, Chat &chat)
{
    in >>
           chat.id        >>
           chat.type      >>
           chat.title     >>
           chat.username  >>
           chat.firstname >>
           chat.lastname;
    return in;
}
}

#endif // CHAT_H
