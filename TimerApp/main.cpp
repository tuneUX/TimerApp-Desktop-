#include <QApplication>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>
#include "TimerApp.h"
#include "UsageDataAccessObject.h"
#include "ScheduleDataAccessObject.h"

int main(int argc, char* argv[]) {  
    QApplication app(argc, argv);

    qDebug() << "Available drivers:" << QSqlDatabase::drivers();
    qDebug() << "Plugin paths:" << QCoreApplication::libraryPaths();

    // Open one SQLite database file
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("timerapp.db");   // persistent file
    if (!db.open()) {
        qDebug() << "DB open error:" << db.lastError().text();
        return -1;
    }

    // Create both tables once
    UsageDataAccessObject::createTable(db);
    ScheduleDataAccessObject::createTable(db);

    // Pass the single DB into TimerApp
    TimerApp window(db);
    window.show();

    return app.exec();
}
