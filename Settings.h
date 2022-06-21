#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include "Database.h"

#define SETTINGS_FILE_NAME "bot_settings"

class Settings : public QObject
{
    Q_OBJECT
public:
    explicit Settings(Database &db, QObject *parent = nullptr); // FIXME 1 arg make QList<>
    void write_settings();

    QString   m_token;
private:
    void read_settings();

signals:

private:
    Database& m_db;

};

#endif // SETTINGS_H
