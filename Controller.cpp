#include "Controller.h"

Controller::Controller(QObject *parent)
    : QObject{parent}
{
    load_db();
}

void Controller::load_db()
{ // при запуске загрузить информацию по тем, кто пользуется ботом. Если БД не существует, создать

}
