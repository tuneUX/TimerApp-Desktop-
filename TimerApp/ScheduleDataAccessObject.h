#pragma once
#include <QSqlDatabase>
#include <QString>
#include <QList>
#include <QVariantMap>

class ScheduleDataAccessObject {
public:
    static void createTable(QSqlDatabase& db);
    static void insert(QSqlDatabase& db, const QString& app, int weekday, int limitSeconds);
    static int getLimit(QSqlDatabase& db, const QString& app, int weekday);
    static QList<QVariantMap> getAll(QSqlDatabase& db);
    static void updateRemaining(
        QSqlDatabase& db,
        const QString& app,
        int weekday,
        int remainingSeconds);
    static void resetDailyLimits(QSqlDatabase& db);



};

