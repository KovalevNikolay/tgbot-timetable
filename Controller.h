#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QMap>
#include <QtConcurrent>
#include <QSet>
#include <QFutureSynchronizer>
#include <QByteArray>
#include <QDataStream>
#include "Database.h"
#include "Settings.h"

#define CMD_START         "/start"
#define CMD_SCHOOL        "/school"
#define CMD_SETSCHOOL     "/setSchool"
#define CMD_STUDENT       "/student"
#define CMD_TEACHER       "/teacher"
#define CMD_SETCLASS      "/setClass"
#define CMD_SETTEACHER    "/setTeacher"
#define CMD_TODAY         "/today"
#define CMD_TOMORROW      "/tomorrow"
#define CMD_WEEK          "/week"
#define CMD_SETTINGS      "/Settings"
#define CMD_NOTIFICATIONS "/notifications"
#define CMD_PLUSVERSION   "/plus"
#define CMD_BACK          "/back"
#define CMD_RESETSETTINGS "/resetSettings" //==CMD_START

#define CMD_INSERT         "/insertSchedule"
#define CMD_UPDATEUSERS    "/updateUsers"
#define CMD_UPDATESCHEDULE "/updateSchedule"
#define CMD_DELETEUSER     "/deleteUser"
#define CMD_DELETESCHEDULE "/deleteSchedule"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);
    ~Controller();

signals:
    void update_id(QString);
    void update_fn(QString);
    void update_username(QString);

private:
    User& find_or_create(const user_id id);
    void load_db();
    void load_db_users();
    void load_db_schedule();
    void load_list_school();
    void load_list_classes();
    void load_list_teachers();

    void updateDataToUsers();
    void updateDataToSchedule();
    void insertUserToDb(const User &user);
    void insertScheduleToDb(const Schedule &schedule);
    void insertMsgToHistory(const User &user);
    void getScheduleOnDay(const int dayOfWeekNumber, const User &user);
    void getScheduleOnWeek (const User &user);

    void processingTheSchool(const User &user);
    void processingTheStudent(const User &user);
    void processingTheTeacher(const User &user);
    void processingTheSetTeacher(const uint32_t &id, User user);
    void processingTheSetClass(const uint32_t &id, User user);
    void processingTheSetSchool(const uint32_t &id, User user);

    void handle_msg_guest(const User &user);
    void handle_msg_normal(const User &user);
    void handle_msg_admin(const User &user);

    QString           msg_to_hex(const Telegram::Message &msg) const;
    Telegram::Message msg_from_hex(const QString &hex)         const;

private slots:
    void handle_msg(const Telegram::Message msg);
    void handle_update(const Telegram::Update upd);

private:
    QMap<user_id,  User>     m_users;
    QMap<uint32_t, Schedule> m_schedule;
    QMap<uint32_t, School>   m_school;
    QMap<uint32_t, Classes>  m_classes;
    QMap<uint32_t, Teacher>  m_teacher;

    Database            m_db_users;
    Database            m_db_schedule;

    Telegram::Bot      *m_bot;
    Settings           *m_settings;

    //Database db_ ? // TODO database school data?

public slots:
    void update_bot_info();
};

#endif // CONTROLLER_H
