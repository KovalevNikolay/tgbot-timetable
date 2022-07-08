#ifndef AUDIO_H
#define AUDIO_H

#include <QDebug>
#include <QString>
#include <QJsonObject>
#include <QDataStream>

namespace Telegram {

class Audio
{
public:
    Audio() {}
    Audio(QJsonObject audio);

    QString fileId;
    quint64 duration;
    QString performer;
    QString title;
    QString mimeType;
    quint64 fileSize;
};

inline QDebug operator<< (QDebug dbg, const Audio &audio)
{
    dbg.nospace() << qUtf8Printable(QString("Telegram::Audio(fileId=%1; duration=%2; performer=%3; title=%4; mimeType=%5; fileSize=%6)")
                                    .arg(audio.fileId)
                                    .arg(audio.duration)
                                    .arg(audio.performer)
                                    .arg(audio.title)
                                    .arg(audio.mimeType)
                                    .arg(audio.fileSize));

    return dbg.maybeSpace();
}
inline QDataStream &operator << (QDataStream &out, const Audio &audio)
{
    out                    <<
           audio.fileId    <<
           audio.duration  <<
           audio.performer <<
           audio.title     <<
           audio.title     <<
           audio.mimeType  <<
           audio.fileSize;
    return out;
}
inline QDataStream &operator >> (QDataStream &in, Audio &audio)
{
    in                     >>
           audio.fileId    >>
           audio.duration  >>
           audio.performer >>
           audio.title     >>
           audio.title     >>
           audio.mimeType  >>
           audio.fileSize;
    return in;
}
}

#endif // AUDIO_H
