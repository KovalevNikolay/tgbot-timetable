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
    void update_id(QString);
    void update_fn(QString);
    void update_username(QString);

private:
    void load_db();

private slots:
    void handle_msg(const Telegram::Message msg);

private:
    Telegram::Bot *m_bot;
    QString        m_token;

    Database       m_db_users;
    //Database db_ ? // TODO database school data?

public slots:
    void update_bot_info();
};

#endif // CONTROLLER_H
