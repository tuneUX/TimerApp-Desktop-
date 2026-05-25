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

class TimerApp : public QMainWindow
{
    Q_OBJECT

public:
    explicit TimerApp(QWidget* parent = nullptr);
    

private slots:
    
    void onStartClicked();
    void onStopClicked();
    void onTogglePauseClicked();
    void CheckActiveWindow();
    void onTrackAppClicked();
    void updateRemaining(int seconds);


private:
    QString timeFormat(int seconds);
    QWidget* central;
    QVBoxLayout* layout;
    QPushButton* startButton;
    QPushButton* stopButton;
    QPushButton* togglePause;
    QPushButton* trackButton;
    QLabel* statusLabel;
    QLabel* timerDisplay;
    QLineEdit* secondsInput;
    QListWidget* openedApps;
    QListWidget* onTimerApps;
    std::unordered_map<QString, TimerLogic*> appTimers;
    //calendar
    QCalendarWidget* calendar;
    void markSpecialDays(int year, int month);
    



   
};
