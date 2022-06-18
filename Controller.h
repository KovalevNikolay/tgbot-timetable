#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include "Database.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

signals:

private:
    void load_db();

private:
    Database db_users;
    //Database db_ ? // TODO database school data?
};

#endif // CONTROLLER_H
