#ifndef LOCATION_H
#define LOCATION_H

#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QDataStream>

namespace Telegram {

class Location
{
public:
    Location() {}
    Location(QJsonObject location);

    float longitude;
    float latitude;
};

inline QDebug operator<< (QDebug dbg, const Location &location)
{
    dbg.nospace() << qUtf8Printable(QString("Telegram::Location(longitude=%1; latitude=%2)")
                                    .arg(location.longitude)
                                    .arg(location.latitude));
    return dbg.maybeSpace();
}
inline QDataStream &operator << (QDataStream &out, const Location &location)
{
    out                       <<
           location.longitude <<
           location.latitude;
    return out;
}
inline QDataStream &operator >> (QDataStream &in, Location &location)
{
    in                        >>
           location.longitude >>
           location.latitude;
    return in;
}
}

#endif // LOCATION_H
