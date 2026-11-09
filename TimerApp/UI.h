#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QListWidget>
#include <QCalendarWidget>

class UI
{
public:
    QPushButton* startButton;
    QPushButton* stopButton;
    QPushButton* togglePause;
    QPushButton* trackButton;
    QPushButton* scheduleButton;

    QLineEdit* secondsInput;

    QLabel* statusLabel;
    QLabel* timerDisplay;

    QListWidget* openedApps;
    QListWidget* onTimerApps;
    QListWidget* usageList;

    QCalendarWidget* calendar;

    QWidget* setupUi(QWidget* parent);
    void applyStyle(QWidget* widget);
};