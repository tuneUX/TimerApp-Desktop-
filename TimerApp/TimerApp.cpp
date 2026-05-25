#include "TimerApp.h"
#include "ActiveWindowHelper.h"
#include "keyHelper.h"
#include <unordered_map>
#include <QString>
#include <QFileInfo>

TimerApp::TimerApp(QWidget* parent) : QMainWindow(parent) {
    startButton = new QPushButton("Start", this);
    stopButton = new QPushButton("Stop", this);
    togglePause = new QPushButton("Pause", this);
    trackButton = new QPushButton("Track App", this);
    secondsInput = new QLineEdit(this);
    statusLabel = new QLabel("Ready", this);
    timerDisplay = new QLabel("Ready", this);
    openedApps = new QListWidget(this);
    onTimerApps = new QListWidget(this);
    calendar = new QCalendarWidget(this);

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

    QWidget* central = new QWidget(this);
    central->setLayout(mainLayout);
    setCentralWidget(central);


    calendar->setGridVisible(true);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    

    // Connections
    connect(startButton, &QPushButton::clicked, this, &TimerApp::onStartClicked);
    connect(stopButton, &QPushButton::clicked, this, &TimerApp::onStopClicked);
    connect(togglePause, &QPushButton::clicked, this, &TimerApp::onTogglePauseClicked);
    connect(trackButton, &QPushButton::clicked, this, &TimerApp::onTrackAppClicked);

    // Refresh open apps list
    QTimer* refreshApps = new QTimer(this);
    connect(refreshApps, &QTimer::timeout, this, [=]() {
        openedApps->clear();
        for (const QString& app : ActiveWindowHelper::getAllOpenProcesses()) {
            openedApps->addItem(app);
        }
        });
    refreshApps->start(3000);

    //calendar call


    markSpecialDays(calendar->selectedDate().year(), calendar->selectedDate().month());
    connect(calendar, &QCalendarWidget::currentPageChanged,

        this, [=](int year, int month) {
            markSpecialDays(year, month);
        }

    );


    // Focus check
    QTimer* focusCheck = new QTimer(this);
    connect(focusCheck, &QTimer::timeout, this, &TimerApp::CheckActiveWindow);
    focusCheck->start(1000);
}

//special Days calendar;
void TimerApp::markSpecialDays(int year, int month) {
    QTextCharFormat highLight;
    highLight.setBackground(Qt::darkBlue);
    highLight.setFontWeight(QFont::Bold);

    QList<int> days = { 10,15,20,25,30 };

    for  ( int day : days)
    {
        QDate date(year, month, day);

        if (date.isValid()) { calendar->setDateTextFormat(date, highLight);  }
    }
    int lastDay = QDate(year, month, 1).daysInMonth();
    QDate lastDate(year, month, lastDay);
    calendar->setDateTextFormat(lastDate, highLight);

}

// Format time as hh:mm:ss
QString TimerApp::timeFormat(int totalSeconds) {
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    return QString("%1:%2:%3")
        .arg(hours, 2, 10, QChar('0'))
        .arg(minutes, 2, 10, QChar('0'))
        .arg(seconds, 2, 10, QChar('0'));
}

void TimerApp::onStartClicked() {
    bool ok;
    int secs = secondsInput->text().toInt(&ok);
    if (!ok || secs <= 0) {
        statusLabel->setText("Enter valid seconds");
        return;
    }

    // Get the selected tracked app
    QListWidgetItem* item = onTimerApps->currentItem();
    if (!item) {
        statusLabel->setText("Select a tracked app first");
        return;
    }

    QString key = item->text().split(" > ").first(); // key before " > seconds"

    if (appTimers.contains(key)) {
        appTimers[key]->start(secs);
        statusLabel->setText("Timer started: " + key);

        // Show immediately
        updateRemaining(secs);
    }
    else {
        statusLabel->setText("No tracked app found for " + key);
    }

    QTextCharFormat startFmt;
    startFmt.setBackground(Qt::darkYellow);
    startFmt.setFontWeight(QFont::Bold);
    startFmt.setForeground(Qt::cyan);
    QDate today = QDate::currentDate();
    calendar->setDateTextFormat(today, startFmt);


}


void TimerApp::onTogglePauseClicked() {
    if (togglePause->text() == "Pause") {
        for (auto& [name, t] : appTimers) t->pause();
        togglePause->setText("Resume");
    }
    else {
        for (auto& [name, t] : appTimers) t->resume();
        togglePause->setText("Pause");
    }
}

void TimerApp::updateRemaining(int seconds) {
   
        timerDisplay->setText(timeFormat(seconds)); 
    
}

void TimerApp::onStopClicked() {
    for (auto& [name, t] : appTimers) t->stop();
    statusLabel->setText("Stopped");
}

void TimerApp::CheckActiveWindow() {
    QString activeProcess = ActiveWindowHelper::getActiveProcessName();
    QString activeTitle = ActiveWindowHelper::getActiveWindowTitle();
    QString key = makeKey(activeProcess, activeTitle);

    bool foundActive = false;
    for (auto& [name, t] : appTimers) {
        if (name == key) {
            t->resume();
            foundActive = true;
        }
        else {
            t->pause();
        }
    }

    statusLabel->setText(
        foundActive
        ? "Active: " + key
        : "Paused (no tracked app in focus)"
             );

    qDebug() << "Active process:" << activeProcess;
    qDebug() << "Active title:"   << activeTitle;
    qDebug() << "Active key:"     << key;

    for (auto& [name, t] : appTimers) {
        qDebug() << "Tracked key:" << name;
    }

}

void TimerApp::onTrackAppClicked() {
    QListWidgetItem* item = openedApps->currentItem();
    if (!item) {
        statusLabel->setText("Select an app first");
        return;
    }

    QString appEntry = item->text();
    QStringList parts = appEntry.split(" > ");
    QString processPath = parts[0].trimmed();
    QString windowTitle = (parts.size() > 1 ? parts[1].trimmed() : parts[0].trimmed());

    QString key = makeKey(processPath, windowTitle);

    // Toggle: detrack if already tracked
    if (appTimers.contains(key)) {
        auto it = appTimers.find(key);
        it->second->stop();
        delete it->second;
        appTimers.erase(it);

        for (int i = 0; i < onTimerApps->count(); i++) {
            if (onTimerApps->item(i)->text().startsWith(key)) {
                delete onTimerApps->takeItem(i);
                break;
            }
        }

        statusLabel->setText("Detracked " + key);
        trackButton->setText("Track App");
        return;
    }

    // Track new app
    auto timer = new TimerLogic(this);
    connect(timer, &TimerLogic::remainingChanged, this, &TimerApp::updateRemaining);

    connect(timer, &TimerLogic::remainingChanged, this, [=](int sec) {
        for (int i = 0; i < onTimerApps->count(); i++) {
            if (onTimerApps->item(i)->text().startsWith(key)) {
                onTimerApps->item(i)->setText(key + " > " + QString::number(sec) + "s");
            }
        }   
        });

    connect(timer, &TimerLogic::finished, this, [=]() {
        statusLabel->setText(key + " finished");
        });
    connect(timer, &TimerLogic::paused, this, [=]() {
        statusLabel->setText(key + " paused");
        });

    appTimers[key] = timer;
    onTimerApps->addItem(key);
    statusLabel->setText("Tracking " + key);
    trackButton->setText("Detrack App");
}
