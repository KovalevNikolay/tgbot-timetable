#ifndef TYPES_H
#define TYPES_H
#include <qttelegrambot.h>
#include <chrono>
#include <QDateTime>

using user_id = qint64;
using tp      = std::chrono::time_point<std::chrono::steady_clock>;

#define now_tp              std::chrono::steady_clock::now()
#define tp_to_sec(x)        std::chrono::time_point_cast<std::chrono::seconds>(x)

struct User
{
    User() = delete;
    User(const Telegram::User &User): tg_user(User), userStatus(Status::regular) {}
    void updateMsg(const Telegram::Message &msg) { last_msg = msg; last_msg_tp = msg.date; list_msg << msg; isNeedUpdate = true; };

    Telegram::User tg_user;
    bool           is_banned    { false };
    QDateTime      ban_tp;
    bool           isNeedUpdate { false };
    bool           isNewUser    { false };

    enum class Role   { teacher, student };
    enum class Status { admin,   donater, regular } userStatus;
    struct settingsRole
    {
        uint32_t      schoolID;
        Role          role;
        uint32_t      roleID;
    } userRole;

    Telegram::Message        last_msg;
    QDateTime                last_msg_tp;
    QList<Telegram::Message> list_msg;
};
struct Schedule
{
    bool              isNeedUpdate { false };
    uint32_t          scheduleID, schoolID, classID, subjectID, teacherID, lessonID, weekDay, audienceNumber;
};
struct School
{
    uint32_t          schoolID;
    QString           schoolName;
};
struct Classes
{
    uint32_t          classID;
    QString           className;

};
struct Teacher
{
    uint32_t          teacherID, schoolID;
    QString           firstName, secondName, lastName;
};

#endif // TYPES_H
