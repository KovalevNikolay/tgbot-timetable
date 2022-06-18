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

private slots:
    void handle_msg(const Telegram::Message msg);

private:
    Telegram::Bot *m_bot;
    QString        m_token;

    Database       m_db_users;
    //Database db_ ? // TODO database school data?
};

#endif // CONTROLLER_H
