#pragma once

#include <QtWidgets/QMainWindow>
#include <QSqlDatabase>
#include <QSystemTrayIcon>
#include <QDate>
#include <unordered_map>
#include<QApplication>

#include "TimerLogic.h"
#include "UsageDataAccessObject.h"
#include "ScheduleManager.h"
#include "ScheduleDataAccessObject.h"
#include "UI.h"

class TimerApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit TimerApp(QSqlDatabase& db, QWidget* parent = nullptr);

private slots:
    void onStartClicked();
    void onStopClicked();
    void onTogglePauseClicked();
    void CheckActiveWindow();
    void onTrackAppClicked();
    void updateRemaining(int seconds);
    void onScheduleClicked();

private:
    QString timeFormat(int seconds);

    // UI
    UI* ui = nullptr;
    QSystemTrayIcon* trayIcon = nullptr;

    // Timer management
    std::unordered_map<QString, TimerLogic*> appTimers;
    ScheduleManager schedule;

    // Database
    QSqlDatabase& dataBase;

    // Usage
    void showUsageData();

    // Calendar
    QDate lastCheckedDate;
    void markScheduledDays(int year, int month);
    void showScheduleForDate(const QDate& date);
};