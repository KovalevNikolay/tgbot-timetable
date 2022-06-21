#include "Settings.h"

Settings::Settings(Database &db, QObject *parent)
    : QObject{parent}, m_db(db)
{
    read_settings();
}

void Settings::read_settings()
{
    QDir current_dir(QDir::currentPath());
    if(!current_dir.exists(SETTINGS_FILE_NAME))
    {
        qDebug() << "Settings path: " << current_dir.absolutePath() << " settings file \"" SETTINGS_FILE_NAME "\" not found. Write default settings...";
        m_token            = "1234567890:DeafaltYourTokenAbCdEfTG7i72InT-L4Sz";
        m_db.m_login_db    = "";
        m_db.m_password_db = "";
        write_settings();
    }

    QSettings s(SETTINGS_FILE_NAME, QSettings::Format::IniFormat);
    s.beginGroup("Token");
    m_token            = s.value("token").toString();
    s.endGroup();
    s.beginGroup("Database");
    m_db.m_login_db    = s.value("db_login").toString();
    m_db.m_password_db = s.value("db_password").toString();
    s.endGroup();
    qDebug() << "Settings has been readed";
}

void Settings::write_settings()
{
    //QSettings s("REGEDIT PATH", QSettings::Format::Registry64Format); // NOTE if u need use WIN REGEDIT
    QSettings s(SETTINGS_FILE_NAME, QSettings::Format::IniFormat);
    s.beginGroup("Token");
    s.setValue("token", m_token);
    s.endGroup();
    s.beginGroup("Database");
    s.setValue("db_login",    m_db.m_login_db);
    s.setValue("db_password", m_db.m_password_db);
    s.endGroup();
    qDebug() << "Settings has been writen";
}
