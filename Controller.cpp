#include "Controller.h"

Controller::Controller(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<Telegram::Message>("Message");
    load_db();

    m_bot = new Telegram::Bot(/*m_token*/"5453283933:AAE29S8gGaYmbKRJQvYkZgT7i02InT-T6Sk", true, 500, 4);
    QObject::connect(m_bot, &Telegram::Bot::message, this,  &Controller::handle_msg);
}

void Controller::load_db()
{ // при запуске загрузить информацию по тем, кто пользуется ботом. Если БД не существует, создать, ну эта логика уже внутри класса Database

}

void Controller::handle_msg(const Telegram::Message msg)
{
    qDebug() << msg;

    auto &user = m_db_users.find_or_create(msg.from.id);
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
    emit update_fn(me.username);
    emit update_username(me.firstname);
    qDebug() << me;
}
