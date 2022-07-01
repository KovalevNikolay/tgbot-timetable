#include "Database.h"

Database::Database(QObject *parent)
    : QObject{parent}
{

}
QSqlError Database::connect_db(const QString &name)
{
    QMutexLocker lock(&m_mtx);
    QDir path_db(QDir::currentPath().append("/").append(name));
    if(!path_db.exists(path_db.absolutePath())) qCritical() << "Not found: " << path_db.absolutePath();

    db = QSqlDatabase::addDatabase("QSQLITE", name);
    db.setDatabaseName(path_db.absolutePath());
    if(m_login_db.isEmpty() ^ m_password_db.isEmpty()) qWarning() << "Login or password db is empty, but other not empty, try connect.."; // FIXME maybe change text warning /nothink
    db.open();

    return db.lastError();
}
void Database::disconnect_db()
{
    QMutexLocker lock(&m_mtx);
    qDebug() << db.databaseName() << " disconnected";
    db.close();
}
QList<QSqlRecord> Database::sql_request(const QString &request)
{
    QMutexLocker lock(&m_mtx);
    QList<QSqlRecord> res;
    QSqlQuery query = QSqlQuery(db);
    if(!query.exec(request)) qCritical() << query.lastError().text();
    else while(query.next()) res << query.record(); // FIXME maybe change while to do while, because query.next() ?
    return res;
}
