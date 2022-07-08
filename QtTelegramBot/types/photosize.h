#ifndef PHOTOSIZE_H
#define PHOTOSIZE_H

#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QDataStream>

namespace Telegram {

class PhotoSize
{
public:
    PhotoSize() {}
    PhotoSize(QJsonObject photoSize);

    QString fileId;
    quint16 width;
    quint16 height;
    quint64 fileSize;
};

inline QDebug operator<< (QDebug dbg, const PhotoSize &photoSize)
{
    dbg.nospace() << qUtf8Printable(QString("Telegram::PhotoSize(fileId=%1; width=%2; height=%3; fileSize=%4)")
                                    .arg(photoSize.fileId)
                                    .arg(photoSize.width)
                                    .arg(photoSize.height)
                                    .arg(photoSize.fileSize));
    return dbg.maybeSpace();
}
inline QDataStream &operator << (QDataStream &out, const PhotoSize &ps)
{
    out                <<
           ps.fileId   <<
           ps.width    <<
           ps.height   <<
           ps.fileSize;
    return out;
}
inline QDataStream &operator >> (QDataStream &in, PhotoSize &ps)
{
    in                 >>
           ps.fileId   >>
           ps.width    >>
           ps.height   >>
           ps.fileSize;
    return in;
}
}

#endif // PHOTOSIZE_H
