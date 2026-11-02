#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_TimerApp.h"
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "TimerLogic.h"
#include <QLineEdit>
#include <QFileInfo>
#include <unordered_map>
#include <QListWidget>
#include<QCalendarWidget>
#include<QSqlDatabase>
#include"UsageDataAccessObject.h"
#include "ScheduleManager.h"
#include "ScheduleDataAccessObject.h"
#include <QSystemTrayIcon>
#include <QDate>

class TimerApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit TimerApp(QSqlDatabase& db,QWidget* parent = nullptr);
    

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
    QWidget* central;
    QVBoxLayout* layout;
    QPushButton* startButton;
    QPushButton* stopButton;
    QPushButton* togglePause;
    QPushButton* trackButton;
    QPushButton* scheduleButton;
    QSystemTrayIcon* trayIcon = nullptr;

    QLabel* statusLabel;
    QLabel* timerDisplay;
    QLineEdit* secondsInput;
    QListWidget* openedApps;
    QListWidget* onTimerApps;
    std::unordered_map<QString, TimerLogic*> appTimers;
    ScheduleManager schedule;
    

    //overuse
    void showUsageData();
    QListWidget* usageList;
    //calendar
    QCalendarWidget* calendar;
    
    QSqlDatabase& dataBase;

    QDate lastCheckedDate;
    

    void markScheduledDays(int year, int month);
    void showScheduleForDate(const QDate& date);

   
};
