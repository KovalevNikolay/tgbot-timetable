#include "Controller.h"

Controller::Controller(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<Telegram::Message>("Message");

    m_settings = new Settings(m_db_users);
    load_db();

    m_bot = new Telegram::Bot(m_settings->m_token, true, 500, 4);
    QObject::connect(m_bot, &Telegram::Bot::message, this,  &Controller::handle_msg);
}
Controller::~Controller()
{
    updateDataToUsers();
    updateDataToSchedule();
}
User& Controller::find_or_create(const user_id id)
{
    auto it = m_users.find(id);
    if(it != m_users.end()) return *it;
    else
    {
        Telegram::User user(id);
        it = m_users.insert(id, user);
        it->isNeedUpdate = true;
        qInfo() << "User created " << user;

        User user_db(user);

        return *it;
    }
}
void Controller::load_db()
{
    QFutureSynchronizer<void> sync;
    sync.addFuture(QtConcurrent::run([this](){ load_db_users(); }));
    sync.addFuture(QtConcurrent::run([this](){ load_db_schedule();}));
}
void Controller::load_db_users()
{
    qInfo() << "Load database users";
    m_db_users.connect_db("users.db"); // FIXME use returned value

    Telegram::User user_from_db;
    User     user(user_from_db);

    for (auto &el : m_db_users.sql_request("SELECT * FROM users"))
    {
       user.tg_user.id        = el.field("user_id").value().toUInt();
       user.tg_user.firstname = el.field("firstName").value().toString();
       user.tg_user.lastname  = el.field("lastName").value().toString();
       user.tg_user.username  = el.field("userName").value().toString();
       user.last_msg_tp = QDateTime::fromSecsSinceEpoch(el.field("last_msg_tp").value().toUInt());
       user.is_banned   = el.field("is_banned").value().toBool();
       user.ban_tp      = QDateTime::fromSecsSinceEpoch(el.field("ban_tp").value().toUInt());
       //user.last_msg=m_db_users.sql_request("SELECT * FROM users")[i].field("last_msg").value().toString();

       m_users.insert(user.tg_user.id, user);
    }
}
void Controller::load_db_schedule()
{
    qInfo() << "Load database school";
    m_db_schedule.connect_db("schedule.db");

    Schedule schedule;

    for (auto &el : m_db_schedule.sql_request("SELECT * FROM schedule"))
    {
        schedule.scheduleID=el.field("ID").value().toInt();
        schedule.classID=el.field("classID").value().toInt();
        schedule.subjectID=el.field("subjectID").value().toInt();
        schedule.teacherID=el.field("teacherID").value().toInt();
        schedule.lessonID=el.field("lessonID").value().toInt();
        schedule.weekDay=el.field("weekDay").value().toInt();
        schedule.audienceNumber=el.field("audienceNumber").value().toInt();

        m_schedule.insert(schedule.scheduleID, schedule);
    }
}
void Controller::updateDataToUsers()
{
    for (auto &el : m_users)
    {
        if (el.isNeedUpdate)
        {
            QString request = "UPDATE users"
                              "SET userName = '"+el.tg_user.username+"', "
                              "firstName = '"+el.tg_user.firstname+"', "
                              "lastName = '"+el.tg_user.lastname+"', "
                              "userStatus = "+static_cast<int>(el.userStatus)+", "
                              "is_banned = "+QString::number(el.is_banned)+", "
                              "ban_tp = "+el.ban_tp.toSecsSinceEpoch()+", "
                              "role = '"+static_cast<int>(el.userRole.role)+"', "
                              "roleID = "+el.userRole.roleID+", "
                              "last_msg_tp = "+el.last_msg_tp.toSecsSinceEpoch()+" "
                              "last_msg = '"+el.last_msg.string+"' " //last_msg
                              "WHERE user_id = "+el.tg_user.id+";";

            m_db_users.sql_request(request);
            el.isNeedUpdate = false;
        }
    }
}
void Controller::updateDataToSchedule()
{
    for (auto &el : m_schedule)
    {
        if (el.isNeedUpdate)
        {
            QString request = "UPDATE schedule"
                              "SET classID = "+QString::number(el.classID)+", "
                              "subjectID = "+QString::number(el.subjectID)+", "
                              "teacherID = "+QString::number(el.teacherID)+", "
                              "lessonID = "+QString::number(el.lessonID)+", "
                              "weekDay = "+QString::number(el.weekDay)+", "
                              "audienceNumber = "+QString::number(el.audienceNumber)+" "
                              "WHERE ID = "+QString::number(el.scheduleID)+";";

            m_db_schedule.sql_request(request);
            el.isNeedUpdate = false;
        }
    }
}
void Controller::insertUserToDb(const User &user)
{
    QString request="INSERT INTO users (userID, userName, firstName, lastName, userStatus, isBanned, banTp, role, roleID, lastMsgTp, lastMsg)"
                    "VALUES ("+QString::number(user.tg_user.id)+", "
                              +user.tg_user.username+", "
                              +user.tg_user.firstname+", "
                              +user.tg_user.lastname+", "
                              +static_cast<int>(user.userStatus)+", "
                              +user.is_banned+", "
                              +user.ban_tp.toSecsSinceEpoch()+", "
                              +static_cast<int>(user.userRole.role)+", "
                              +QString::number(user.userRole.roleID)+", "
                              +user.last_msg_tp.toSecsSinceEpoch()+", "
                              +user.last_msg.string+");"; //last_msg

    m_db_users.sql_request(request);
}
void Controller::insertScheduleToDb(const Schedule &schedule)
{
    QString request="INSERT INTO schedule (classID, subjectID, teacherID, lessonID, weekDay, audienceNumber)"
                    "VALUES ("+QString::number(schedule.classID)+", "
                              +QString::number(schedule.subjectID)+", "
                              +QString::number(schedule.teacherID)+", "
                              +QString::number(schedule.lessonID)+", "
                              +QString::number(schedule.weekDay)+", "
                              +QString::number(schedule.audienceNumber)+");";

    m_db_schedule.sql_request(request);
}
void Controller::getScheduleOnDay(const int weekDayNumber, const User::settingsRole &settings)
{
    QString request;
    if (settings.role == User::Role::student)
    {
        request="SELECT schedule.lessonID, Lessons.LessonTime, subjects.SubjectName, teachers.FirstName, teachers.SecondName, teachers.LastName, schedule.audienceNumber "
                "FROM schedule "
                    "INNER JOIN classes ON schedule.classID = classes.classID "
                    "INNER JOIN subjects ON schedule.subjectID = subjects.subjectID "
                    "INNER JOIN teachers ON schedule.teacherID = teachers.teacherID "
                    "INNER JOIN calendar ON schedule.weekDay = calendar.weekDay "
                    "INNER JOIN lessons ON schedule.lessonID = lessons.lessonID "
                "WHERE schedule.weekDay = "+QString::number(weekDayNumber)+" "
                      "AND classes.className= '"+QString::number(settings.roleID)+"' "
                "ORDER BY schedule.LessonID AND schedule.weekDay ";

        m_db_schedule.sql_request(request);
    }
    else if (settings.role == User::Role::teacher)
    {
        request="SELECT schedule.lessonID, Lessons.LessonTime, subjects.SubjectName, classes.className, schedule.audienceNumber "
                "FROM schedule "
                    "INNER JOIN classes ON schedule.classID = classes.classID "
                    "INNER JOIN subjects ON schedule.subjectID = subjects.subjectID "
                    "INNER JOIN teachers ON schedule.teacherID = teachers.teacherID "
                    "INNER JOIN calendar ON schedule.weekDay = calendar.weekDay "
                    "INNER JOIN lessons ON schedule.lessonID = lessons.lessonID "
                "WHERE schedule.weekDay = "+QString::number(weekDayNumber)+" "
                      "AND teachers.teacherID= "+QString::number(settings.roleID)+" "
                "ORDER BY schedule.LessonID AND schedule.weekDay ";

        m_db_schedule.sql_request(request);
    }
}
void Controller::getScheduleOnWeek(const User::settingsRole &settings)
{
    QString request;
    if (settings.role == User::Role::student)
    {
        request="SELECT schedule.lessonID, Lessons.LessonTime, subjects.SubjectName, teachers.FirstName, teachers.SecondName, teachers.LastName, schedule.audienceNumber "
                "FROM schedule "
                    "INNER JOIN classes ON schedule.classID = classes.classID "
                    "INNER JOIN subjects ON schedule.subjectID = subjects.subjectID "
                    "INNER JOIN teachers ON schedule.teacherID = teachers.teacherID "
                    "INNER JOIN calendar ON schedule.weekDay = calendar.weekDay "
                    "INNER JOIN lessons ON schedule.lessonID = lessons.lessonID "
                "WHERE classes.className= '"+QString::number(settings.roleID)+"' "
                "ORDER BY schedule.LessonID AND schedule.weekDay ";
        m_db_schedule.sql_request(request);
    }
    else if (settings.role == User::Role::teacher)
    {
        request="SELECT schedule.lessonID, Lessons.LessonTime, subjects.SubjectName, classes.className, schedule.audienceNumber "
                "FROM schedule "
                    "INNER JOIN classes ON schedule.classID = classes.classID "
                    "INNER JOIN subjects ON schedule.subjectID = subjects.subjectID "
                    "INNER JOIN teachers ON schedule.teacherID = teachers.teacherID "
                    "INNER JOIN calendar ON schedule.weekDay = calendar.weekDay "
                    "INNER JOIN lessons ON schedule.lessonID = lessons.lessonID "
                "WHERE teachers.teacherID= "+QString::number(settings.roleID)+" "
                "ORDER BY schedule.LessonID AND schedule.weekDay ";
        m_db_schedule.sql_request(request);
    }
}
void Controller::handle_msg(const Telegram::Message msg)
{
    qDebug() << msg;

    auto &user = find_or_create(msg.from.id);
    user.updateMsg(msg);

    if(!user.is_banned)
    {
        switch(msg.type)
        {
            case Telegram::Message::TextType:
            {
                m_bot->sendMessage(user.tg_user.id, user.last_msg.string + " last msg time: " + user.last_msg_tp.toString("dd.MM.yyyy hh:mm:ss"));
                break;
            }
            case Telegram::Message::AudioType: break;
            case Telegram::Message::DocumentType: break;
            case Telegram::Message::StickerType: break;
            case Telegram::Message::VideoType: break;
            case Telegram::Message::VoiceType: break;
            case Telegram::Message::ContactType: break;
            case Telegram::Message::LocationType: break;
            case Telegram::Message::NewChatParticipantType: break;
            case Telegram::Message::LeftChatParticipantType: break;
            case Telegram::Message::NewChatTitleType: break;
            case Telegram::Message::NewChatPhotoType: break;
            case Telegram::Message::DeleteChatPhotoType: break;
            case Telegram::Message::GroupChatCreatedType: break;
            default: break;
        }
    } else
    {
        qDebug() << "Banned " << user.tg_user << " send message";
    }


    // TODO
}
void Controller::update_bot_info()
{
    auto me = m_bot->getMe();
    emit update_id(QString::number(me.id));
    emit update_fn(me.firstname);
    emit update_username(me.username);
    qDebug() << me;
}
