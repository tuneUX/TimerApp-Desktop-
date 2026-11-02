#include "DBConnection.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

QSqlDatabase DBConnection::getUsageDB() {
    if (!QSqlDatabase::contains("usage")) {
        QSqlDatabase usage = QSqlDatabase::addDatabase("QSQLITE", "usage");
        qDebug() << "isValid:" << usage.isValid();
        qDebug() << "driverName:" << usage.driverName();
        qDebug() << "drivers list:" << QSqlDatabase::drivers();
        usage.setDatabaseName("usage.db");
        if (!usage.open()) {
            qDebug() << "Usage DB error:" << usage.lastError().text();
        }
    }
    return QSqlDatabase::database("usage");
}

QSqlDatabase DBConnection::getScheduleDB() {
    if (!QSqlDatabase::contains("schedule")) {
        QSqlDatabase schedule = QSqlDatabase::addDatabase("QSQLITE", "schedule");
        schedule.setDatabaseName("schedule.db");
        if (!schedule.open()) {
            qDebug() << "Schedule DB error:" << schedule.lastError().text();
        }
    }
    return QSqlDatabase::database("schedule");
}
