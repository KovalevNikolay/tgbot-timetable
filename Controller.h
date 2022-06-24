#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QMap>
#include <QtConcurrent>
#include <QFutureSynchronizer>
#include "Database.h"
#include "Settings.h"

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(QObject *parent = nullptr);

signals:
    void update_id(QString);
    void update_fn(QString);
    void update_username(QString);

private:
    User& find_or_create(const user_id id);
    void  load_db();
    void  load_db_users();
    void  load_db_school();
    void  write_users_to_db();

private slots:
    void handle_msg(const Telegram::Message msg);

private:
    QMap<user_id, User> m_users;
    // TODO impl me m_school_data;
    // TODO impl me m_other_data; // if need

    Database            m_db_users;
    Database            m_db_school;

    Telegram::Bot      *m_bot;
    Settings           *m_settings;

    //Database db_ ? // TODO database school data?

public slots:
    void update_bot_info();
};

#endif // CONTROLLER_H
