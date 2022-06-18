#include "Controller.h"

Controller::Controller(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<Telegram::Message>("Message");
    load_db();

    m_bot = new Telegram::Bot(/*m_token*/"5453283933:AAE29S8gGaYmbKRJQvYkZgT7i02InT-T6Sk", true, 500, 4);
    QObject::connect(m_bot, &Telegram::Bot::message, this,  &Controller::handle_msg);

    auto me = m_bot->getMe();
    qDebug() << me;
}

void Controller::load_db()
{ // при запуске загрузить информацию по тем, кто пользуется ботом. Если БД не существует, создать, ну эта логика уже внутри класса Database

}

void Controller::handle_msg(const Telegram::Message msg)
{
    qDebug() << msg;

    switch(msg.type)
    {
        //case: break;
    }

    // TODO
}
