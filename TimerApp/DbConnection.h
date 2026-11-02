#pragma once
#include <QSqlDatabase>

// Helper class to manage multiple database connections
class DBConnection {
public:
    // Returns the usage database connection (usage.db)
    static QSqlDatabase getUsageDB();

    // Returns the schedule database connection (schedule.db)
    static QSqlDatabase getScheduleDB();
};
