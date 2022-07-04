#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QMap>
#include <QtConcurrent>
#include <QSet>
#include <QFutureSynchronizer>
#include "Database.h"
#include "Settings.h"

#define CMD_START "/start"
#define CMD_STUDENT "/student"
#define CMD_TEACHER "/teacher"
#define CMD_TODAY "/today"
#define CMD_TOMORROW "/tomorrow"
#define CMD_WEEK "/week"
#define CMD_CHANGESCHOOL "/changeSchool"
#define CMD_CHANGESETTINGS "/changeSettings"

#define CMD_INSERT "/insertSchedule"
#define CMD_UPDATEUSERS "/updateUsers"
#define CMD_UPDATESCHEDULE "/updateSchedule"
#define CMD_DELETEUSER "/deleteUser"
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
    void load_table_school();
    void updateDataToUsers();
    void updateDataToSchedule();
    void insertUserToDb(const User &user);
    void insertScheduleToDb(const Schedule &schedule);
    void getScheduleOnDay(const int weekDayNumber, const User::settingsRole &settings);
    void getScheduleOnWeek (const User::settingsRole &settings);

    void processingTheStudent();
    void processingTheTeacher();
    void processingTheToday();
    void processingTheTomorrow();
    void processingTheWeek();
    void processingTheChangeSchool();
    void processingTheChangeSettings();

    void handle_msg_guest(const User &user);
    void handle_msg_normal(const User &user);
    void handle_msg_admin(const User &user);


private slots:
    void handle_msg(const Telegram::Message msg);

private:
    QMap<user_id, User> m_users;
    QMap<uint32_t, Schedule> m_schedule;
    QMap<uint32_t, School> m_school;

    Database            m_db_users;
    Database            m_db_schedule;

    Telegram::Bot      *m_bot;
    Settings           *m_settings;

    //Database db_ ? // TODO database school data?

public slots:
    void update_bot_info();
};

#endif // CONTROLLER_H
