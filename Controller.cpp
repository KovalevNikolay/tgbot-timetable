#include "Controller.h"

Controller::Controller(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<Telegram::Message>("Message");
    qRegisterMetaType<Telegram::Update>("Update");

    m_settings = new Settings(m_db_users);
    load_db();

    m_bot = new Telegram::Bot(m_settings->m_token, true, 500, 4);
    QObject::connect(m_bot, &Telegram::Bot::update,  this,  &Controller::handle_update);
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
        qInfo() << "User created " << user;

        insertUserToDb(User(user));

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
    auto res = m_db_users.connect_db("users.db"); // FIXME use returned value
    if(res.isValid())
    {
        qCritical() << "users database connection error";
    	return;
    }

    Telegram::User user_from_db;
    User     user(user_from_db);
    QString  request;

    for(auto &el : m_db_users.sql_request("SELECT * FROM users"))
    {
       user.tg_user.id              = el.field("userID").value().toUInt();
       user.last_msg.chat.username  = el.field("userName").value().toString();
       user.last_msg.chat.firstname = el.field("firstName").value().toString();
       user.last_msg.chat.lastname  = el.field("lastName").value().toString();
       user.userRole.schoolID       = el.field("schoolID").value().toUInt();
       user.userRole.roleID         = el.field("roleID").value().toUInt();
       user.userRole.role           = static_cast<User::Role>(el.field("role").value().toUInt());
       user.userStatus              = static_cast<User::Status>(el.field("userStatus").value().toUInt());
       user.last_msg_tp             = QDateTime::fromSecsSinceEpoch(el.field("last_msg_tp").value().toUInt());
       user.is_banned               = el.field("is_banned").value().toBool();
       user.ban_tp                  = QDateTime::fromSecsSinceEpoch(el.field("ban_tp").value().toUInt());
       user.last_msg                = msg_from_hex(el.field("last_msg").value().toString());

       request = "SELECT hex_msg FROM historyMsg WHERE userID ="+QString::number(user.tg_user.id);
       for (auto &it : m_db_users.sql_request(request)) user.list_msg.push_back(msg_from_hex(it.field("hex_msg").value().toString()));

       m_users.insert(user.last_msg.chat.id, user);
    }
}
void Controller::load_db_schedule()
{
    qInfo() << "Load database schedule";
    auto res = m_db_schedule.connect_db("schedule.db");
    if(res.isValid())
    {
        qCritical() << "schedule database connection error";
    	return;
    }

    Schedule schedule;

    for(auto &el : m_db_schedule.sql_request("SELECT * FROM schedule"))
    {
        schedule.scheduleID     = el.field("ID").value().toUInt();
        schedule.schoolID       = el.field("schoolID").value().toUInt();
        schedule.classID        = el.field("classID").value().toUInt();
        schedule.subjectID      = el.field("subjectID").value().toUInt();
        schedule.teacherID      = el.field("teacherID").value().toUInt();
        schedule.lessonID       = el.field("lessonID").value().toUInt();
        schedule.weekDay        = el.field("weekDay").value().toUInt();
        schedule.audienceNumber = el.field("audienceNumber").value().toUInt();

        m_schedule.insert(schedule.scheduleID, schedule);
    }

    load_list_school();
    load_list_classes();
    load_list_teachers();
}
void Controller::load_list_school()
{
    qInfo() << "Load list school";
    School school;

    for(auto &el : m_db_schedule.sql_request("SELECT * FROM schools"))
    {
        school.schoolID   = el.field("schoolID").value().toUInt();
        school.schoolName = el.field("schoolName").value().toString();

        m_school.insert(school.schoolID, school);
    }
}
void Controller::load_list_classes()
{
    qInfo() << "Load list classes";
    Classes classes;

    for(auto &el : m_db_schedule.sql_request("SELECT * FROM classes"))
    {
        classes.classID   = el.field("classID").value().toUInt();
        classes.className = el.field("className").value().toString();

        m_classes.insert(classes.classID, classes);
    }
}
void Controller::load_list_teachers()
{
    qInfo() << "Load list teachers";
    Teacher teacher;

    for(auto &el : m_db_schedule.sql_request("SELECT * FROM teachers"))
    {
        teacher.teacherID  = el.field("teacherID").value().toUInt();
        teacher.schoolID   = el.field("schoolID").value().toUInt();
        teacher.firstName  = el.field("firstName").value().toString();
        teacher.secondName = el.field("secondName").value().toString();
        teacher.lastName   = el.field("lastName").value().toString();

        m_teacher.insert(teacher.teacherID, teacher);
    }
}
void Controller::updateDataToUsers()
{
    for(auto &el : m_users)
    {
        if(el.isNeedUpdate)
        {
            QString request = "UPDATE users "
                              "SET "
                              "userName = '"    + el.last_msg.chat.username                           + "', "
                              "firstName = '"   + el.last_msg.chat.firstname                          + "', "
                              "lastName = '"    + el.last_msg.chat.lastname                           + "', "
                              "userStatus = "   + QString::number(static_cast<int>(el.userStatus))    + ", "
                              "is_banned = "    + QString::number(el.is_banned)                       + ", "
                              "ban_tp = "       + QString::number(el.ban_tp.toSecsSinceEpoch())       + ", "
                              "schoolID = "     + QString::number(el.userRole.schoolID)               + ", "
                              "role = "         + QString::number(static_cast<int>(el.userRole.role)) + ", "
                              "roleID = "       + QString::number(el.userRole.roleID)                 + ", "
                              "last_msg_tp = "  + QString::number(el.last_msg_tp.toSecsSinceEpoch())  + ", "
                              "last_msg = '"    + msg_to_hex(el.last_msg)                             + "' "
                              "WHERE userID = " + QString::number(el.tg_user.id)                      + ";";

            m_db_users.sql_request(request);
            el.isNeedUpdate = false;
        }
    }
}
void Controller::updateDataToSchedule()
{
    for(auto &el : m_schedule)
    {
        if(el.isNeedUpdate)
        {
            QString request = "UPDATE schedule "
                              "SET schoolID = "   + QString::number(el.schoolID)       + ", "
                              "classID = "        + QString::number(el.classID)        + ", "
                              "subjectID = "      + QString::number(el.subjectID)      + ", "
                              "teacherID = "      + QString::number(el.teacherID)      + ", "
                              "lessonID = "       + QString::number(el.lessonID)       + ", "
                              "weekDay = "        + QString::number(el.weekDay)        + ", "
                              "audienceNumber = " + QString::number(el.audienceNumber) + " "
                              "WHERE ID = "       + QString::number(el.scheduleID)     + ";";

            m_db_schedule.sql_request(request);
            el.isNeedUpdate = false;
        }
    }
}
void Controller::insertUserToDb(const User &user)
{
    QString request = "INSERT INTO users (userID, userName, firstName, lastName, userStatus, is_banned, ban_tp, schoolID, role, roleID, last_msg_tp, last_msg) "
                      "VALUES ("+QString::number(user.tg_user.id)                      + ", '"
                                +user.last_msg.chat.username                           + "', '"
                                +user.last_msg.chat.firstname                          + "', '"
                                +user.last_msg.chat.lastname                           + "', "
                                +QString::number(static_cast<int>(user.userStatus))    + ", "
                                +QString::number(user.is_banned)                       + ", "
                                +QString::number(user.ban_tp.toSecsSinceEpoch())       + ", "
                                +QString::number(user.userRole.schoolID)               + ", "
                                +QString::number(static_cast<int>(user.userRole.role)) + ", "
                                +QString::number(user.userRole.roleID)                 + ", "
                                +QString::number(user.last_msg_tp.toSecsSinceEpoch())  + ", '"
                                +msg_to_hex(user.last_msg)                             + "');";

    m_db_users.sql_request(request);
}
void Controller::insertScheduleToDb(const Schedule &schedule)
{
    QString request = "INSERT INTO schedule (schoolID, classID, subjectID, teacherID, lessonID, weekDay, audienceNumber) "
                      "VALUES ("+QString::number(schedule.schoolID)       + ", "
                                +QString::number(schedule.classID)        + ", "
                                +QString::number(schedule.subjectID)      + ", "
                                +QString::number(schedule.teacherID)      + ", "
                                +QString::number(schedule.lessonID)       + ", "
                                +QString::number(schedule.weekDay)        + ", "
                                +QString::number(schedule.audienceNumber) + ");";

    m_db_schedule.sql_request(request);
}
void Controller::insertMsgToHistory(const User &user)
{
    QString request = "INSERT INTO historyMsg (userID, hex_msg)"
                      "VALUES ("+QString::number(user.tg_user.id)+", '"+msg_to_hex(user.last_msg)+"');";
    m_db_users.sql_request(request);
}
QString Controller::getScheduleOnDay(const int dayOfWeekNumber, const User &user)
{
    QString request="", textMessage="";
    if(user.userRole.role == User::Role::student)
    {
        request = "SELECT schedule.lessonID, Lessons.LessonTime, subjects.SubjectName, teachers.FirstName, teachers.SecondName, teachers.LastName, schedule.audienceNumber "
                  "FROM schedule "
                    "INNER JOIN schools ON schedule.schoolID = schools.schoolID "
                    "INNER JOIN classes ON schedule.classID = classes.classID "
                    "INNER JOIN subjects ON schedule.subjectID = subjects.subjectID "
                    "INNER JOIN teachers ON schedule.teacherID = teachers.teacherID "
                    "INNER JOIN calendar ON schedule.weekDay = calendar.weekDay "
                    "INNER JOIN lessons ON schedule.lessonID = lessons.lessonID "
                  "WHERE schedule.weekDay = "   + QString::number(dayOfWeekNumber)        + " "
                    "AND classes.classID = " + QString::number(user.userRole.roleID)      + " "
                    "AND schools.schoolID = "   + QString::number(user.userRole.schoolID) + " "
                  "ORDER BY schedule.LessonID AND schedule.weekDay ";

        for(auto &el : m_db_schedule.sql_request(request))
        {
            textMessage += el.field("lessonID").value().toString()                    + " урок"
                    "\nВремя урока: "    + el.field("lessonTime").value().toString()  +
                    "\nПредмет: "        + el.field("SubjectName").value().toString() +
                    "\nПреподаватель: "  + el.field("firstName").value().toString()   +
                    " " +el.field("secondName").value().toString()                    +
                    " " +el.field("lastName").value().toString()                      +
                    "\nКабинет: "        + el.field("audienceNumber").value().toString()+"\n\n";
        }
    }
    else if(user.userRole.role == User::Role::teacher)
    {
        request = "SELECT schedule.lessonID, Lessons.LessonTime, subjects.SubjectName, classes.className, schedule.audienceNumber "
                  "FROM schedule "
                    "INNER JOIN schools ON schedule.schoolID = schools.schoolID "
                    "INNER JOIN classes ON schedule.classID = classes.classID "
                    "INNER JOIN subjects ON schedule.subjectID = subjects.subjectID "
                    "INNER JOIN teachers ON schedule.teacherID = teachers.teacherID "
                    "INNER JOIN calendar ON schedule.weekDay = calendar.weekDay "
                    "INNER JOIN lessons ON schedule.lessonID = lessons.lessonID "
                  "WHERE schedule.weekDay = "   + QString::number(dayOfWeekNumber)       + " "
                    "AND teachers.teacherID = " + QString::number(user.userRole.roleID)  + " "
                    "AND schools.schoolID = "   + QString::number(user.userRole.schoolID)+ " "
                  "ORDER BY schedule.LessonID AND schedule.weekDay ";

        for(auto &el : m_db_schedule.sql_request(request))
        {
            textMessage += el.field("lessonID").value().toString()                    + " урок"
                    "\nВремя урока: " + el.field("lessonTime").value().toString()     +
                    "\nПредмет: "     + el.field("SubjectName").value().toString()    +
                    "\nКласс: "       + el.field("className").value().toString()      +
                    "\nКабинет: "     + el.field("audienceNumber").value().toString() + "\n\n";
        }
    }

    return textMessage;
}
QString Controller::getScheduleOnWeek(const User &user)
{
    QString textMessage="";
    for (int i=1; i<7; i++)
        if (getScheduleOnDay(i, user)!="")
            textMessage+=QDate::longDayName(i, QDate::StandaloneFormat)+"\n\n"+getScheduleOnDay(i, user)+"***********\n\n";

    return textMessage;
}
void Controller::studentOrTeacher(const User &user)
{
    Telegram::InlineKeyboardButtons btns;
    btns << Telegram::InlineKeyboardButton("Student", "", "callback choice " + QString::number(0));
    btns << Telegram::InlineKeyboardButton("Teacher", "", "callback choice " + QString::number(1));

    m_bot->sendMessage(user.tg_user.id, "Are you a student or teacher?", false, false, -1, Telegram::InlineKeyboardMarkup(btns));

}
void Controller::processingTheSchool(const User &user)
{
    Telegram::InlineKeyboardButtons btns;
    for(const auto &el : qAsConst(m_school)) btns << Telegram::InlineKeyboardButton(el.schoolName, "", "callback school " + QString::number(el.schoolID));

    m_bot->sendMessage(user.tg_user.id, "Select your school:", false, false, -1, Telegram::InlineKeyboardMarkup(btns));
}
void Controller::processingTheStudent(const User &user)
{
    Telegram::InlineKeyboardButtons btns;
    for(const auto &el : qAsConst(m_classes)) btns << Telegram::InlineKeyboardButton(el.className, "", "callback class " + QString::number(el.classID));

    m_bot->sendMessage(user.tg_user.id, "Select your class:", false, false, -1, Telegram::InlineKeyboardMarkup(btns));
}
void Controller::processingTheTeacher(const User &user)
{
    Telegram::InlineKeyboardButtons btns;
    for (const auto &el :qAsConst(m_teacher)) if (user.userRole.schoolID == el.schoolID) btns << Telegram::InlineKeyboardButton(el.firstName + " " +el.secondName + " " + el.lastName, "", "callback teacher " +QString::number(el.teacherID));

    m_bot->sendMessage(user.tg_user.id, "Select your teacher:", false, false, -1, Telegram::InlineKeyboardMarkup(btns));
}
void Controller::setTeacher(const uint32_t &id, User &user)
{
    user.userRole.role   = User::Role::teacher;
    user.userRole.roleID = id;
}
void Controller::setClass(const uint32_t &id, User &user)
{
    user.userRole.role   = User::Role::student;
    user.userRole.roleID = id;
}
void Controller::setSchool(const uint32_t &id, User &user)
{
    user.userRole.schoolID = id;
}
void Controller::functionalMenu(const User &user)
{
    Telegram::InlineKeyboardButtons btns;
    btns << Telegram::InlineKeyboardButton("Today", "", "callback menu " + QString::number(1));
    btns << Telegram::InlineKeyboardButton("Tomorrow", "", "callback menu " + QString::number(2));
    btns << Telegram::InlineKeyboardButton("A week", "", "callback menu " + QString::number(3));
    btns << Telegram::InlineKeyboardButton("Reset settings", "", "callback menu " + QString::number(4));

    m_bot->sendMessage(user.tg_user.id, "Menu", false, false, -1, Telegram::InlineKeyboardMarkup(btns));
}
void Controller::handle_msg_guest(const User &user)
{
    const auto &msg = user.last_msg;
    switch(msg.type)
    {
        case Telegram::Message::TextType:
        {
            if(msg.string == CMD_START)          processingTheSchool(user);
            else if(msg.string == CMD_TODAY)     getScheduleOnDay(QDate::currentDate().dayOfWeek(), user);
            else if(msg.string == CMD_TOMORROW)  getScheduleOnDay(QDate::currentDate().dayOfWeek()+1, user);
            else if(msg.string == CMD_WEEK)      getScheduleOnWeek(user); // пока что не трогать
            else if(msg.string == CMD_SETTINGS)
            {

            }
            else if(msg.string == CMD_NOTIFICATIONS)
            {

            }
            else if(msg.string == CMD_PLUSVERSION)
            {

            }
            else if(msg.string == CMD_BACK)
            {

            }
            else if(msg.string == CMD_RESETSETTINGS)
            {

            }
            break;
        }
        // save
        case Telegram::Message::AudioType: break;
        case Telegram::Message::DocumentType: break;
        case Telegram::Message::StickerType:
        {
            m_bot->sendMessage(user.tg_user.id, "Ты нахуя стикер кинул еблан");
            break;
        }
        case Telegram::Message::VideoType: break;
        case Telegram::Message::VoiceType: break;
        case Telegram::Message::ContactType: break;
        case Telegram::Message::LocationType: break;
        // nothink
        case Telegram::Message::NewChatParticipantType: break;
        case Telegram::Message::LeftChatParticipantType: break;
        case Telegram::Message::NewChatTitleType: break;
        case Telegram::Message::NewChatPhotoType: break;
        case Telegram::Message::DeleteChatPhotoType: break;
        case Telegram::Message::GroupChatCreatedType: break;
        default: break;
    }
}
void Controller::handle_msg_normal(const User &user)
{
    const auto msg = user.last_msg;
    switch(msg.type)
    {
        case Telegram::Message::TextType:
        {
            //m_bot->sendMessage(user.tg_user.id, user.last_msg.string + " last msg time: " + user.last_msg_tp.toString("dd.MM.yyyy hh:mm:ss"));
            break;
        }
        // save
        case Telegram::Message::AudioType: break;
        case Telegram::Message::DocumentType: break;
        case Telegram::Message::StickerType:
        {
            m_bot->sendMessage(user.tg_user.id, "Ты нахуя стикер кинул еблан");
            break;
        }
        case Telegram::Message::VideoType: break;
        case Telegram::Message::VoiceType: break;
        case Telegram::Message::ContactType: break;
        case Telegram::Message::LocationType: break;
        // nothink
        case Telegram::Message::NewChatParticipantType: break;
        case Telegram::Message::LeftChatParticipantType: break;
        case Telegram::Message::NewChatTitleType: break;
        case Telegram::Message::NewChatPhotoType: break;
        case Telegram::Message::DeleteChatPhotoType: break;
        case Telegram::Message::GroupChatCreatedType: break;
        default: break;
    }
}
void Controller::handle_msg_admin(const User &user)
{
    const auto msg = user.last_msg;
    switch(msg.type)
    {
        case Telegram::Message::TextType:
        {
            //m_bot->sendMessage(user.tg_user.id, user.last_msg.string + " last msg time: " + user.last_msg_tp.toString("dd.MM.yyyy hh:mm:ss"));
            break;
        }
        // save
        case Telegram::Message::AudioType: break;
        case Telegram::Message::DocumentType: break;
        case Telegram::Message::StickerType: break;
        case Telegram::Message::VideoType: break;
        case Telegram::Message::VoiceType: break;
        case Telegram::Message::ContactType: break;
        case Telegram::Message::LocationType: break;
        // nothink
        case Telegram::Message::NewChatParticipantType: break;
        case Telegram::Message::LeftChatParticipantType: break;
        case Telegram::Message::NewChatTitleType: break;
        case Telegram::Message::NewChatPhotoType: break;
        case Telegram::Message::DeleteChatPhotoType: break;
        case Telegram::Message::GroupChatCreatedType: break;
        default: break;
    }
}
QString Controller::msg_to_hex(const Telegram::Message &msg) const
{
    QByteArray  bytes;
    QDataStream ds(&bytes, QIODevice::WriteOnly);
    ds << msg;
    return QString(bytes.toHex());
}
Telegram::Message Controller::msg_from_hex(const QString &hex) const
{
    QByteArray  bytes = QByteArray::fromHex(hex.toUtf8());
    QDataStream ds(&bytes, QIODevice::ReadOnly);
    Telegram::Message msg;
    ds >> msg;
    return msg;
}
void Controller::handle_update(const Telegram::Update upd)
{
    if(!(upd.message.id && upd.callbackquery.message.id)) qCritical() << "no way wtf";
    auto msg = (upd.message.id)?upd.message:(upd.callbackquery.message);
    qDebug() << msg;
    auto &user = (msg.from.id)?find_or_create(msg.from.id):find_or_create(upd.callbackquery.from.id);
    qDebug() << "CB ID" << upd.callbackquery.from.id;
    qDebug() << "USER ID" << user.tg_user.id;

    user.updateMsg(msg);
    insertMsgToHistory(user);

    if(!user.is_banned)
    {
        if(user.userStatus == User::Status::regular)      handle_msg_guest(user);
        else if(user.userStatus == User::Status::donater) handle_msg_normal(user);
        else if(user.userStatus == User::Status::admin)   handle_msg_admin(user);
    } else qInfo() << "Banned " << user.tg_user << " send message";

    //CALLBACK
    const auto cb = upd.callbackquery;
    if(!cb.isEmpty())
    {
        qDebug() << cb;
        m_bot->answerCallbackQuery(cb.id);
        if(cb.data.indexOf("callback school") != -1)
        {
            constexpr auto substr_size = sizeof("callback school") - 1;
            QStringRef ref(&cb.data, substr_size, cb.data.size() - substr_size);
            ref = ref.trimmed();
            setSchool(ref.toUInt(), user);

            studentOrTeacher(user);

        } else if (cb.data.indexOf("callback choice") != -1)
        {
            constexpr auto substr_size = sizeof("callback choice") - 1;
            QStringRef ref(&cb.data, substr_size, cb.data.size() - substr_size);
            if (ref.trimmed().toInt()) processingTheTeacher(user);
            else                       processingTheStudent(user);

        } else if(cb.data.indexOf("callback class") != -1)
        {
            constexpr auto substr_size = sizeof("callback class") - 1;
            QStringRef ref(&cb.data, substr_size, cb.data.size() - substr_size);
            ref = ref.trimmed();
            setClass(ref.toUInt(), user);
            functionalMenu(user);

        } else if(cb.data.indexOf("callback teacher") != -1)
        {
            constexpr auto substr_size = sizeof("callback teacher") - 1;
            QStringRef ref(&cb.data, substr_size, cb.data.size() - substr_size);
            ref = ref.trimmed();
            setTeacher(ref.toUInt(), user);
            functionalMenu(user);

        } else if (cb.data.indexOf("callback menu") != -1)
        {
            constexpr auto substr_size = sizeof("callback menu") - 1;
            QStringRef ref(&cb.data, substr_size, cb.data.size() - substr_size);
            switch(ref.trimmed().toUInt())
            {
                case 1:
                {
                    m_bot->sendMessage(user.tg_user.id, getScheduleOnDay(QDate::currentDate().dayOfWeek(), user));
                    functionalMenu(user);
                    break;
                }
                case 2:
                {
                    m_bot->sendMessage(user.tg_user.id, getScheduleOnDay(QDate::currentDate().dayOfWeek()+1, user));
                    functionalMenu(user);
                    break;
                }
                case 3:
                {
                    m_bot->sendMessage(user.tg_user.id, getScheduleOnWeek(user));
                    functionalMenu(user);
                    break;
                }
                case 4:
                {
                    processingTheSchool(user);
                    break;
                }
            }
        }
        else qWarning() << "Unknown callback: " << cb.data;
    }
}
void Controller::update_bot_info()
{
    auto me = m_bot->getMe();
    emit update_id(QString::number(me.id));
    emit update_fn(me.firstname);
    emit update_username(me.username);
    qDebug() << me;
}
