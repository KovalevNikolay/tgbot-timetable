#ifndef TYPES_H
#define TYPES_H
#include <qttelegrambot.h>
#include <chrono>

using tp = std::chrono::time_point<std::chrono::steady_clock>;

struct User
{
    Telegram::User tgUser;
    bool           isBanned;
    tp             lastMsg;
};

#endif // TYPES_H
