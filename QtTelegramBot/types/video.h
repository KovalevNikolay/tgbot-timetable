#ifndef VIDEO_H
#define VIDEO_H

#include <QString>
#include <QJsonObject>
#include <QDataStream>
#include "photosize.h"

namespace Telegram {

class Video
{
public:
    Video() {}
    Video(QJsonObject video);

    QString   fileId;
    quint16   width;
    quint16   height;
    quint64   duration;
    PhotoSize thumb;
    QString   mimeType;
    QString   fileSize;
};

inline QDebug operator<< (QDebug dbg, const Video &video)
{
    dbg.nospace() << qUtf8Printable(QString("Telegram::Video(fileId=%1; width=%2; height=%3; duration=%4; thumb=%5; mimeType=%6; fileSize=%7)")
                                    .arg(video.fileId)
                                    .arg(video.width)
                                    .arg(video.height)
                                    .arg(video.duration)
                                    .arg("PhotoSize(" + video.thumb.fileId + ")")
                                    .arg(video.mimeType)
                                    .arg(video.fileSize));
    return dbg.maybeSpace();
}
inline QDataStream &operator << (QDataStream &out, const Video &video)
{
    out                   <<
           video.fileId   <<
           video.width    <<
           video.height   <<
           video.duration <<
           video.thumb    <<
           video.mimeType <<
           video.fileSize;
    return out;
}
inline QDataStream &operator >> (QDataStream &in, Video &video)
{
    in                    >>
           video.fileId   >>
           video.width    >>
           video.height   >>
           video.duration >>
           video.thumb    >>
           video.mimeType >>
           video.fileSize;
    return in;
}
}

#endif // VIDEO_H
