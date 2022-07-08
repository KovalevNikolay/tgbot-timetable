#ifndef MESSAGE_H
#define MESSAGE_H

#include <QDebug>

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include <QDataStream>

#include "audio.h"
#include "document.h"
#include "photosize.h"
#include "sticker.h"
#include "video.h"
#include "voice.h"
#include "contact.h"
#include "location.h"
#include "chat.h"
#include "user.h"

namespace Telegram {

class Message
{
public:
    Message() {}
    Message(QJsonObject message);

    /**
     * @brief Telegram message events
     */
    enum MessageType : quint8
    {
        TextType, AudioType, DocumentType, PhotoType, StickerType, VideoType, VoiceType, ContactType,
        LocationType, NewChatParticipantType, LeftChatParticipantType, NewChatTitleType,
        NewChatPhotoType, DeleteChatPhotoType, GroupChatCreatedType
    };

    // required
    quint32   id;
    QDateTime date;
    Chat      chat;

    // optional
    User      from;
    User      forwardFrom;
    QDateTime forwardDate;
    Message  *replyToMessage;

    MessageType type;

    // payload
    QString          string;
    User             user;
    Audio            audio;
    Document         document;
    QList<PhotoSize> photo;
    Sticker          sticker;
    Video            video;
    Voice            voice;
    Contact          contact;
    Location         location;
};

inline QDebug operator<< (QDebug dbg, const Message &message)
{
    dbg.nospace() << qUtf8Printable(QString("Telegram::Message(id=%1; date=%2; chat=%3; type=%4) msg[%5]")
                                    .arg(message.id)
                                    .arg(message.date.toString("dd.MM.yyyy hh:mm:ss"))
                                    .arg("Chat(" + QString::number(message.chat.id) + ")")
                                    .arg(message.type)
                                    .arg((message.type == Message::TextType)? message.string :""));

    return dbg.maybeSpace();
}
inline QDataStream &operator << (QDataStream &out, const Message &msg)
{
    out <<
           msg.id <<
           msg.date <<
           msg.chat <<
           msg.from <<
           msg.forwardFrom <<
           msg.forwardDate <<
           //msg.replyToMessage << // FIXME SEG FAULT RECURSIV CALL
           msg.type <<
           msg.string <<
           msg.user <<
           msg.audio <<
           msg.document <<
           msg.photo <<
           msg.sticker <<
           msg.video <<
           msg.voice <<
           msg.contact <<
           msg.location;
    return out;
}
inline QDataStream &operator >> (QDataStream &in, Message &msg)
{
    in            >>
           msg.id >>
           msg.date >>
           msg.chat >>
           msg.from >>
           msg.forwardFrom >>
           msg.forwardDate >>
           //msg.replyToMessage >> // FIXME SEG FAULT RECURSIV CALL
           msg.type >>
           msg.string >>
           msg.user >>
           msg.audio >>
           msg.document >>
           msg.photo >>
           msg.sticker >>
           msg.video >>
           msg.voice >>
           msg.contact >>
           msg.location;
    return in;
}
}
Q_DECLARE_METATYPE(Telegram::Message)

#endif // MESSAGE_H
