#include "ScheduleDataAccessObject.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFileInfo>

void ScheduleDataAccessObject::createTable(QSqlDatabase& db)
{
    QSqlQuery query(db);

    query.exec(
        "CREATE TABLE IF NOT EXISTS schedule ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "app TEXT NOT NULL,"
        "weekday INTEGER NOT NULL,"
        "original_limit_seconds INTEGER NOT NULL,"
        "remaining_seconds INTEGER NOT NULL,"
        "UNIQUE(app,weekday));");
}

void ScheduleDataAccessObject::insert(
    QSqlDatabase& db,
    const QString& app,
    int weekday,
    int limitSeconds)
{
    QSqlQuery query(db);

    qDebug()
        << "Preparing insert for schedule:"
        << "app=" << app
        << "weekday=" << weekday
        << "limit=" << limitSeconds;

    query.prepare(
        "INSERT OR REPLACE INTO schedule("
        "app,"
        "weekday,"
        "original_limit_seconds,"
        "remaining_seconds)"
        " VALUES("
        ":app,"
        ":weekday,"
        ":original_limit,"
        ":remaining)");

    query.bindValue(":app", app);
    query.bindValue(":weekday", weekday);
    query.bindValue(":original_limit", limitSeconds);
    query.bindValue(":remaining", limitSeconds);

    qDebug() << "Bound values:" << query.boundValues();

    if (!query.exec()) {
        qDebug() << "Insert error:"
            << query.lastError().text();
    }
    else {
        qDebug() << "Insert succeeded";
        qDebug() << "Rows affected:"
            << query.numRowsAffected();
    }

    QSqlQuery verify(db);

    verify.exec(
        "SELECT app,"
        " weekday,"
        " original_limit_seconds,"
        " remaining_seconds "
        "FROM schedule");

    while (verify.next()) {
        qDebug()
            << "APP:" << verify.value(0).toString()
            << "DAY:" << verify.value(1).toInt()
            << "ORIGINAL:" << verify.value(2).toInt()
            << "REMAINING:" << verify.value(3).toInt();
    }

    qDebug() << "Database:"
        << db.databaseName();

    qDebug()
        << QFileInfo(db.databaseName())
        .absoluteFilePath();
}

int ScheduleDataAccessObject::getLimit(
    QSqlDatabase& db,
    const QString& app,
    int weekday)
{
    QSqlQuery query(db);

    query.prepare(
        "SELECT remaining_seconds "
        "FROM schedule "
        "WHERE app=:app "
        "AND weekday=:weekday");

    query.bindValue(":app", app);
    query.bindValue(":weekday", weekday);

    if (query.exec() && query.next()) {
        return query.value(0).toInt();
    }

    return -1;
}

QList<QVariantMap> ScheduleDataAccessObject::getAll(
    QSqlDatabase& db)
{
    QList<QVariantMap> results;

    QSqlQuery query(db);

    query.exec(
        "SELECT app,"
        " weekday,"
        " original_limit_seconds,"
        " remaining_seconds "
        "FROM schedule");

    while (query.next()) {

        QVariantMap row;

        row["app"] =
            query.value(0).toString();

        row["weekday"] =
            query.value(1).toInt();

        row["original_limit_seconds"] =
            query.value(2).toInt();

        row["remaining_seconds"] =
            query.value(3).toInt();

        results.append(row);
    }

    return results;
}
void ScheduleDataAccessObject::updateRemaining(
    QSqlDatabase& db,
    const QString& app,
    int weekday,
    int remainingSeconds)
{
    QSqlQuery query(db);

    query.prepare(
        "UPDATE schedule "
        "SET remaining_seconds = :remaining "
        "WHERE app = :app "
        "AND weekday = :weekday");

    query.bindValue(":remaining", remainingSeconds);
    query.bindValue(":app", app);
    query.bindValue(":weekday", weekday);

    if (!query.exec())
    {
        qDebug() << "Update error:"
            << query.lastError().text();
    }

}

void ScheduleDataAccessObject::resetDailyLimits(QSqlDatabase& db)
{
    QSqlQuery query(db);

    query.prepare(
        "UPDATE schedule "
        "SET remaining_seconds = original_limit_seconds");

    if (!query.exec())
    {
        qDebug() << "Reset error:"
            << query.lastError().text();
    }
    else
    {
        qDebug() << "Daily limits reset";
    }
}