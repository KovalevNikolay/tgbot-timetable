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
        qInfo() << "User created " << user;

        User user_db(user);
        // QtConcurence::run([&user_db](){
        //
        // TODO async write user data to users.db
        //  });

        return *it;
    }
}
void Controller::load_db()
{
    QFutureSynchronizer<void> sync;
    sync.addFuture(QtConcurrent::run([this](){ load_db_users(); }));
    sync.addFuture(QtConcurrent::run([this](){ load_db_school();}));
}
void Controller::load_db_users()
{
    qInfo() << "Load database users";
    m_db_users.connect_db("users.db");
    // TODO fill m_users
    // get count users
    // foreach() m_users.insert
    // etc.
}
void Controller::load_db_school()
{
    qInfo() << "Load database school";
    //m_db_school.connect_db("school.db");
    // TODO
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
