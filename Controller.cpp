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
User& Controller::find_or_create(const user_id id)
{
    auto it = m_users.find(id);
    if(it != m_users.end())
    {
        return *it;
    } else
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
    m_db_users.connect_db("users.db");

    for (int i=0; i<m_db_users.sql_request("SELECT * FROM users").size(); i++)

    {
       Telegram::User user_from_db;

       User user(user_from_db);
       user.tg_user.id=m_db_users.sql_request("SELECT * FROM users")[i].field("user_id").value().toUInt();
       user.tg_user.firstname=m_db_users.sql_request("SELECT * FROM users")[i].field("firstName").value().toString();
       user.tg_user.lastname=m_db_users.sql_request("SELECT * FROM users")[i].field("lastName").value().toString();
       user.tg_user.username=m_db_users.sql_request("SELECT * FROM users")[i].field("userName").value().toString();
       user.last_msg_tp=QDateTime::fromSecsSinceEpoch(m_db_users.sql_request("SELECT * FROM users")[i].field("last_msg_tp").value().toUInt());
       user.is_banned=m_db_users.sql_request("SELECT * FROM users")[i].field("is_banned").value().toBool();
       user.ban_tp=QDateTime::fromSecsSinceEpoch(m_db_users.sql_request("SELECT * FROM users")[i].field("ban_tp").value().toUInt());
       //user.last_msg=m_db_users.sql_request("SELECT * FROM users")[i].field("last_msg").value().toString();

       m_users.insert(user.tg_user.id, user);

    }
}
void Controller::load_db_schedule()
{
    qInfo() << "Load database school";
    m_db_schedule.connect_db("schedule.db");

    Schedule schedule;

    for (int i=0; i<m_db_schedule.sql_request("SELECT * FROM schedule").size(); i++)
    {
        schedule.scheduleID=m_db_schedule.sql_request("SELECT * FROM schedule")[i].field("ID").value().toInt();
        schedule.classID=m_db_schedule.sql_request("SELECT * FROM schedule")[i].field("classID").value().toInt();
        schedule.subjectID=m_db_schedule.sql_request("SELECT * FROM schedule")[i].field("subjectID").value().toInt();
        schedule.teacherID=m_db_schedule.sql_request("SELECT * FROM schedule")[i].field("teacherID").value().toInt();
        schedule.lessonID=m_db_schedule.sql_request("SELECT * FROM schedule")[i].field("lessonID").value().toInt();
        schedule.weekDay=m_db_schedule.sql_request("SELECT * FROM schedule")[i].field("weekDay").value().toInt();
        schedule.audienceNumber=m_db_schedule.sql_request("SELECT * FROM schedule")[i].field("audienceNumber").value().toInt();

        m_schedule.insert(schedule.scheduleID, schedule);
    }
}
void Controller::write_users_to_db()
{
    for (auto &el : m_users)
    {
        if (el.isNeedUpdate)
        {
            QString request = "INSERT INTO users (user_id, firstName, lastName, userName, last_msg_tp, is_banned, ban_tp, last_msg)"
                          "VALUES (:user_id, :firstName, :lastName, :userName, :last_msg_tp, :is_banned, :ban_tp, :last_msg)";
            m_db_users.sql_insert_user(request, el);
        }
    }//Update data SQL request;
}
void Controller::write_schedule_to_db()
{
    for (auto &el : m_schedule)
    {
        QString request = "INSERT INTO schedule (classID, subjectID, teacherID, lessonID, weekDay, audienceNumber) "
                          "VALUES (:classID, :subjectID, :teacherID, :lessonID, :weekDay, :audienceNumber)";
        m_db_schedule.sql_insert_schedule(request, el);
    }
    //it is needed convert classes fields to strings to simplify sql_insert_schedule and sql_insert_users methods
}

void Controller::getScheduleOnDay(const int weekDayNumber, const User::settingsRole &setRole)
{
    QString request;
    if (setRole.roleName=="student")
    {
        request="SELECT schedule.lessonID, Lessons.LessonTime, subjects.SubjectName, teachers.FirstName, teachers.SecondName, teachers.LastName, schedule.audienceNubmer "
                "FROM schedule "
                    "INNER JOIN classes ON schedule.classID = classes.classID "
                    "INNER JOIN subjects ON schedule.subjectID = subjects.subjectID "
                    "INNER JOIN teachers ON schedule.teacherID = teachers.teacherID "
                    "INNER JOIN calendar ON schedule.weekDay = calendar.weekDay "
                    "INNER JOIN lessons ON schedule.lessonID = lessons.lessonID "
                "WHERE schedule.weekDay = "+QString::number(weekDayNumber)+" "
                      "AND classes.className= '"+QString::number(setRole.roleID)+"' "
                "ORDER BY schedule.LessonID AND schedule.weekDay ";
        m_db_schedule.sql_request(request);
    }
    else if (setRole.roleName=="teacher")
    {
        request="SELECT schedule.lessonID, Lessons.LessonTime, subjects.SubjectName, classes.className, schedule.audienceNubmer "
                "FROM schedule "
                    "INNER JOIN classes ON schedule.classID = classes.classID "
                    "INNER JOIN subjects ON schedule.subjectID = subjects.subjectID "
                    "INNER JOIN teachers ON schedule.teacherID = teachers.teacherID "
                    "INNER JOIN calendar ON schedule.weekDay = calendar.weekDay "
                    "INNER JOIN lessons ON schedule.lessonID = lessons.lessonID "
                "WHERE schedule.weekDay = "+QString::number(weekDayNumber)+" "
                      "AND teachers.teacherID= "+QString::number(setRole.roleID)+" "
                "ORDER BY schedule.LessonID AND schedule.weekDay ";
        m_db_schedule.sql_request(request);
    }
}
void Controller::getScheduleOnWeek(const User::settingsRole &setRole)
{
    QString request;
    if (setRole.roleName=="student")
    {
        request="SELECT schedule.lessonID, Lessons.LessonTime, subjects.SubjectName, teachers.FirstName, teachers.SecondName, teachers.LastName, schedule.audienceNubmer "
                "FROM schedule "
                    "INNER JOIN classes ON schedule.classID = classes.classID "
                    "INNER JOIN subjects ON schedule.subjectID = subjects.subjectID "
                    "INNER JOIN teachers ON schedule.teacherID = teachers.teacherID "
                    "INNER JOIN calendar ON schedule.weekDay = calendar.weekDay "
                    "INNER JOIN lessons ON schedule.lessonID = lessons.lessonID "
                "WHERE classes.className= '"+QString::number(setRole.roleID)+"' "
                "ORDER BY schedule.LessonID AND schedule.weekDay ";
        m_db_schedule.sql_request(request);
    }
    else if (setRole.roleName=="teacher")
    {
        request="SELECT schedule.lessonID, Lessons.LessonTime, subjects.SubjectName, classes.className, schedule.audienceNubmer "
                "FROM schedule "
                    "INNER JOIN classes ON schedule.classID = classes.classID "
                    "INNER JOIN subjects ON schedule.subjectID = subjects.subjectID "
                    "INNER JOIN teachers ON schedule.teacherID = teachers.teacherID "
                    "INNER JOIN calendar ON schedule.weekDay = calendar.weekDay "
                    "INNER JOIN lessons ON schedule.lessonID = lessons.lessonID "
                "WHERE teachers.teacherID= "+QString::number(setRole.roleID)+" "
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
