#include "UI.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

QWidget* UI::setupUi(QWidget* parent)
{
    startButton = new QPushButton("Start", parent);
    stopButton = new QPushButton("Stop", parent);
    togglePause = new QPushButton("Pause", parent);
    trackButton = new QPushButton("Track App", parent);
    scheduleButton = new QPushButton("Set Weekly Limit", parent);

    secondsInput = new QLineEdit(parent);

    statusLabel = new QLabel("Ready", parent);
    timerDisplay = new QLabel("Ready", parent);

    openedApps = new QListWidget(parent);
    onTimerApps = new QListWidget(parent);
    usageList = new QListWidget(parent);
    calendar = new QCalendarWidget(parent);

    QVBoxLayout* mainLayout = new QVBoxLayout;
    QHBoxLayout* topBar = new QHBoxLayout;
    QHBoxLayout* listLayout = new QHBoxLayout;

    topBar->addWidget(startButton);
    topBar->addWidget(stopButton);
    topBar->addWidget(togglePause);
    topBar->addWidget(secondsInput);
    topBar->addWidget(trackButton);

    listLayout->addWidget(openedApps);
    listLayout->addWidget(onTimerApps);

    mainLayout->addLayout(topBar);
    mainLayout->addWidget(statusLabel);
    mainLayout->addWidget(timerDisplay);
    mainLayout->addLayout(listLayout);
    mainLayout->addWidget(calendar, 1);
    mainLayout->addWidget(usageList);
    mainLayout->addWidget(scheduleButton);

    QWidget* central = new QWidget(parent);
    central->setLayout(mainLayout);

    calendar->setGridVisible(true);
    calendar->setVerticalHeaderFormat(
        QCalendarWidget::NoVerticalHeader);

    applyStyle(central);

    return central;
}

void UI::applyStyle(QWidget* widget)
{
    widget->setStyleSheet(R"(
        QWidget {
            background-color: #231014;
            color: #EAEAEA;
            font-family: Segoe UI;
            font-size: 10pt;
        }

        QPushButton {
            background-color: #301934;
            color: white;
            border: 1px solid #4B5563;
            border-radius: 6px;
            padding: 8px 14px;
        }

        QPushButton:hover {
            background-color: #465365;
        }

        QPushButton:pressed {
            background-color: #purple;
        }

        QLineEdit, QListWidget {
            background-color: #2B3038;
            color: white;
            border: 1px solid #505967;
            border-radius: 5px;
            padding: 6px;
        }

        QCalendarWidget QWidget {
            background-color: #AA336A;
            color: white;
        }
    )");
}