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
    User(const Telegram::User &User): tg_user(User), userStatus(Status::guest) {}
    void updateMsg(const Telegram::Message &msg) { last_msg = msg; last_msg_tp = msg.date; };

    Telegram::User tg_user;
    bool           is_banned { false };
    QDateTime      ban_tp;
    bool           isNeedUpdate { false };

    enum class Role   { teacher, student };
    enum class Status { admin,   normal, guest } userStatus;
    struct settingsRole
    {
        Role          role;
        int           roleID;
    } userRole;

    Telegram::Message last_msg;
    QDateTime         last_msg_tp;
};
struct Schedule
{
    bool              isNeedUpdate { false };
    uint32_t          scheduleID, classID, subjectID, teacherID, lessonID, weekDay, audienceNumber;
};

#endif // TYPES_H
