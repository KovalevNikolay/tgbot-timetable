#ifndef CONTACT_H
#define CONTACT_H

#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QDataStream>

namespace Telegram {

class Contact
{
public:
    Contact() {}
    Contact(QJsonObject contact);

    QString phoneNumber;
    QString firstname;
    QString lastname;
    quint32 userId;
};

inline QDebug operator<< (QDebug dbg, const Contact &contact)
{
    dbg.nospace() << qUtf8Printable(QString("Telegram::Contact(phoneNumber=%1; firstname=%2; lastname=%3; userId=%4)")
                                    .arg(contact.phoneNumber)
                                    .arg(contact.firstname)
                                    .arg(contact.lastname)
                                    .arg(contact.userId));
    return dbg.maybeSpace();
}
inline QDataStream &operator << (QDataStream &out, const Contact &contact)
{
    out                        <<
           contact.phoneNumber <<
           contact.firstname   <<
           contact.lastname    <<
           contact.userId;
    return out;
}
inline QDataStream &operator >> (QDataStream &in, Contact &contact)
{
    in                         >>
           contact.phoneNumber >>
           contact.firstname   >>
           contact.lastname    >>
           contact.userId;
    return in;
}
}

#endif // CONTACT_H
