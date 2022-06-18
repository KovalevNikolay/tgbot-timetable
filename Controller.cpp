#include "Controller.h"

Controller::Controller(QObject *parent)
    : QObject{parent}
{
    qRegisterMetaType<Telegram::Message>("Message");
    load_db();
}

void Controller::load_db()
{ // при запуске загрузить информацию по тем, кто пользуется ботом. Если БД не существует, создать

}
