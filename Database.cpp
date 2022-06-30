#include "Database.h"

Database::Database(QObject *parent)
    : QObject{parent}
{

}
QSqlError Database::connect_db(const QString &name)
{
    QMutexLocker lock(&m_mtx);
    QDir curr_path(QDir::currentPath());
    if(!curr_path.exists(curr_path.absolutePath() + name)) qCritical() << "Not found: " << curr_path.absolutePath() << name;

    db = QSqlDatabase::addDatabase("QSQLITE", name);
    db.setDatabaseName(curr_path.absolutePath() + name);
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
    if(!query.exec(request))
    {
        qDebug() << query.lastError().databaseText();
        qDebug() << query.lastError().driverText();
    } else while(query.next()) res << query.record(); // FIXME maybe change while to do while, because query.next() ?
    return res;
}
