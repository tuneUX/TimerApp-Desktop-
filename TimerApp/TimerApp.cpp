#include "TimerApp.h"
#include "ActiveWindowHelper.h"
#include "keyHelper.h"

#include <QDialog>
#include <QVBoxLayout>  
#include <QHBoxLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QIcon>
#include <QDebug>
#include <QTimer>
#include <QTextCharFormat>
#include <QFont>
#include <QVariantMap>
#include <QListWidgetItem>


  


TimerApp::TimerApp(QSqlDatabase& db,QWidget* parent) : QMainWindow(parent),dataBase(db) {
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
    usageList = new QListWidget(this);
    scheduleButton = new QPushButton("Set Weekly Limit", this);
    lastCheckedDate = QDate::currentDate();
 


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
    

    QWidget* central = new QWidget(this);
    central->setLayout(mainLayout);
    setCentralWidget(central);


    calendar->setGridVisible(true);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon("clock.png"));
    trayIcon->setToolTip("Timer App");
    trayIcon->show();
    

    // Connections
    connect(startButton, &QPushButton::clicked, this, &TimerApp::onStartClicked);
    connect(stopButton, &QPushButton::clicked, this, &TimerApp::onStopClicked);
    connect(togglePause, &QPushButton::clicked, this, &TimerApp::onTogglePauseClicked);
    connect(trackButton, &QPushButton::clicked, this, &TimerApp::onTrackAppClicked);
    connect(scheduleButton, &QPushButton::clicked, this, &TimerApp::onScheduleClicked);


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

// Highlight scheduled days when the calendar opens or changes month.
    markScheduledDays(
        calendar->yearShown(),
        calendar->monthShown()
    );

    connect(calendar, &QCalendarWidget::currentPageChanged,
        this, [this](int year, int month) {
            markScheduledDays(year, month);
        });

    // Show the schedule for the clicked date.
    connect(calendar, &QCalendarWidget::clicked,
        this, &TimerApp::showScheduleForDate);

    
    // Focus check
    QTimer* focusCheck = new QTimer(this);
    connect(focusCheck, &QTimer::timeout, this, &TimerApp::CheckActiveWindow);
    focusCheck->start(1000);
}

//special Days calendar;
void TimerApp::markScheduledDays(int year, int month) {
    QTextCharFormat normalFormat;
    QTextCharFormat scheduledFormat;

    scheduledFormat.setBackground(Qt::darkBlue);
    scheduledFormat.setForeground(Qt::white);
    scheduledFormat.setFontWeight(QFont::Bold);

    QDate firstDay(year, month, 1);
    if (!firstDay.isValid())
        return;

    for (int day = 1; day <= firstDay.daysInMonth(); ++day) {
        calendar->setDateTextFormat(
            QDate(year, month, day), normalFormat);
    }
    for (int day = 1; day <= firstDay.daysInMonth(); ++day) {
        QDate date(year, month, day);
        int weekday = date.dayOfWeek();
        for (const auto& [appkey, timer] : appTimers) {
            Q_UNUSED(timer);
                
            int limit = ScheduleDataAccessObject::getLimit(dataBase, appkey, weekday);
            if (limit > 0) {
                calendar->setDateTextFormat(date, scheduledFormat);
                break;
            }

        }
    }

}
//showing in calendar
void TimerApp::showScheduleForDate(const QDate& date)
{
    QString message =
        date.toString("dddd, MMMM d, yyyy") + "\n\n";

    int weekday = date.dayOfWeek();
    bool foundSchedule = false;

    for (const auto& [appKey, timer] : appTimers) {
        Q_UNUSED(timer);

        int limit = ScheduleDataAccessObject::getLimit(
            dataBase, appKey, weekday);

        if (limit > 0) {
            message += appKey + ": "
                + timeFormat(limit) + "\n";

            foundSchedule = true;
        }
    }

    if (!foundSchedule) {
        message += "No scheduled timers for this day.";
    }

    QMessageBox::information(
        this,
        "Schedule for Selected Date",
        message
    );
}

//schedule adding management
void TimerApp::onScheduleClicked() {
    QDialog dialog(this);
    dialog.setWindowTitle("Set Weekly Limit");

    QVBoxLayout* vbox = new QVBoxLayout(&dialog);

    QLabel* appLabel = new QLabel("Select App:");
    QComboBox* appCombo = new QComboBox();
    for (int i = 0; i < onTimerApps->count(); ++i) {
        appCombo->addItem(onTimerApps->item(i)->text());
    }

    QLabel* dayLabel = new QLabel("Select Day:");
    QComboBox* dayCombo = new QComboBox();
    dayCombo->addItems({ "Monday","Tuesday","Wednesday","Thursday","Friday" });

    QLabel* timeLabel = new QLabel("Limit (seconds):");
    QLineEdit* timeInput = new QLineEdit();

    QHBoxLayout* hbox = new QHBoxLayout();
    QPushButton* okBtn = new QPushButton("OK");
    QPushButton* cancelBtn = new QPushButton("Cancel");
    hbox->addWidget(okBtn);
    hbox->addWidget(cancelBtn);

    vbox->addWidget(appLabel);
    vbox->addWidget(appCombo);
    vbox->addWidget(dayLabel);
    vbox->addWidget(dayCombo);
    vbox->addWidget(timeLabel);
    vbox->addWidget(timeInput);
    vbox->addLayout(hbox);

    connect(okBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QString appKey = appCombo->currentText();
        int weekday = dayCombo->currentIndex() + 1;
        int limitSeconds = timeInput->text().toInt();
        if (weekday < 1 || weekday > 5) {
            QMessageBox::warning(
                this,
                "Invalid Day",
                "Timers can only be scheduled Monday through Friday."
            );
            return;
        }

        if (limitSeconds <= 0) {
            QMessageBox::warning(
                this,
                "Invalid Limit",
                "Enter a time limit greater than zero."
            );
            return;
        }

        ScheduleDataAccessObject::insert(dataBase, appKey, weekday, limitSeconds);

        statusLabel->setText("Limit set for " + appKey +
            " on " + dayCombo->currentText() +
            " to " + QString::number(limitSeconds) + "s");
        markScheduledDays(
            calendar->yearShown(),
            calendar->monthShown()
        );
    }
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
    QListWidgetItem* item = onTimerApps->currentItem();

    if (!item) {
        statusLabel->setText("Select a tracked app first");
        return;
    }

    QString key = item->text().split(" > ").first();

    bool ok;
    int secs = secondsInput->text().toInt(&ok);

    if (ok && secs > 0) {
        // Quick timer: does not modify scheduled remaining time.
        if (appTimers.contains(key)) {
            appTimers[key]->start(secs);

            statusLabel->setText(
                "Quick timer started: " + key +
                " (" + QString::number(secs) + "s)");

            updateRemaining(secs);
        }
        else {
            statusLabel->setText("No tracked app found for " + key);
        }
    }
    else {
        // Routine timer: load today's saved remaining time.
        int today = QDate::currentDate().dayOfWeek();
        int limit = ScheduleDataAccessObject::getLimit(
            dataBase, key, today);

        if (limit > 0) {
            if (appTimers.contains(key)) {
                appTimers[key]->startScheduled(limit);

                statusLabel->setText(
                    "Routine timer started: " + key +
                    " (" + QString::number(limit) + "s)");

                updateRemaining(limit);
            }
            else {
                statusLabel->setText("No tracked app found for " + key);
            }
        }
        else {
            statusLabel->setText("No DB limit set for " + key);
        }
    }

    // Highlight today in calendar.
    QTextCharFormat startFmt;
    startFmt.setBackground(Qt::darkYellow);
    startFmt.setFontWeight(QFont::Bold);
    startFmt.setForeground(Qt::cyan);

    QDate todayDate = QDate::currentDate();
    calendar->setDateTextFormat(todayDate, startFmt);
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
    if (seconds > 0) {
        timerDisplay->setText("Remaining: " + timeFormat(seconds));
        timerDisplay->setStyleSheet("color: green;");
    }
    else if(seconds < 0){
        int overuse = -seconds;
        timerDisplay->setText("Overuse: " + timeFormat(overuse));
        timerDisplay->setStyleSheet("color: red; font-weight: bold;");
    }
    else { timerDisplay->setStyleSheet("color: white; font-weight: bold;"); }
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

    
    QDate currentDate = QDate::currentDate();
    int today = currentDate.dayOfWeek();
    
    
    
    //reset daily limit
    if (currentDate != lastCheckedDate)
    {
        ScheduleDataAccessObject::resetDailyLimits(
            dataBase);

        lastCheckedDate = currentDate;

        qDebug() << "New day detected - limits reset";
    }

    // Get today’s weekday (Qt: 1=Mon, 5=fri)

    if (today > 5) {
        for (auto& [name, timer] : appTimers) {
            if (timer->isScheduled()) {
                timer->pause();
            }
        }

        statusLabel->setText("No scheduled timers on weekends");
        return;
    }



    
    //.

    for (auto& [name, t] : appTimers) {
        if (name == key) {
            
            
          
            
            int limit = ScheduleDataAccessObject::getLimit(dataBase, key, today);

            if (limit > 0) {
                // enforce daily countdown limit
                if (!t->isRunning()) {
                    t->resume();
                }
                
                statusLabel->setText("Active with limit: " + key +
                    " (" + QString::number(limit) + "s)");
            }
            else {
                // no limit set, just resume
                t->resume();
                statusLabel->setText("Active (no limit): " + key);
            }
            foundActive = true;
        }
        else {
            t->pause();
        }
    }
    
    if (!foundActive) {
        statusLabel->setText("Paused (no tracked app in focus)");
    }

    // Debug logging
    qDebug() << "Active process:" << activeProcess;
    qDebug() << "Active title:" << activeTitle;
    qDebug() << "Active key:" << key;
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
    auto timer = new TimerLogic(dataBase,this);

    connect(timer, &TimerLogic::remainingChanged, this, &TimerApp::updateRemaining);

    connect(timer, &TimerLogic::remainingChanged, this, [=](int sec) {
        for (int i = 0; i < onTimerApps->count(); i++) {
            if (onTimerApps->item(i)->text().startsWith(key)) {
                if (sec >= 0) {
                    onTimerApps->item(i)->setText(key + " > Remaining: " + QString::number(sec) + "s");
                }
                else {
                    int overuse = -sec; // negative means overuse
                    onTimerApps->item(i)->setText(key + " > Overuse: " + QString::number(overuse) + "s");
                    onTimerApps->item(i)->setForeground(Qt::red); // highlight overuse
                }
            }
        }
        int today =
            QDate::currentDate().dayOfWeek();

        if (timer->isScheduled() && sec >= 0) {
            ScheduleDataAccessObject::updateRemaining(
                dataBase, key, today, sec);
        }
        });


    connect(timer, &TimerLogic::finished, this, [=]() {
        statusLabel->setText(key + " finished");
        int overUsedMins = timer->getOveruseMinutes();
        if (overUsedMins > 0) {
            UsageDataAccessObject::insert(dataBase, key, overUsedMins, QDate::currentDate().toString("yyyy-MM-dd"));
            showUsageData();

        }
        QMessageBox::warning(
            this,
            "Timer Finished",
            "The time limit for " + key + " has been reached!"
        );
        if (trayIcon->supportsMessages()) {
            trayIcon->showMessage(
                "Timer Finished",
                "The time limit for " + key + " has been reached!",
                QSystemTrayIcon::Warning,
                5000
            );
        }
        });

    connect(timer, &TimerLogic::paused, this, [=]() {
        statusLabel->setText(key + " paused");
        });

    appTimers[key] = timer;
    onTimerApps->addItem(key);
    statusLabel->setText("Tracking " + key);
    trackButton->setText("Detrack App");
}

void TimerApp::showUsageData() {
    QList<QVariantMap> usageRecords = UsageDataAccessObject::getAll(dataBase);
    usageList->clear();
    for (const auto& row : usageRecords) {
        QString line = QString("%1 (%2 min overuse) on %3")
            .arg(row["app"].toString())
            .arg(row["overuse_min"].toInt())
            .arg(row["date"].toString());
        usageList->addItem(line);
    }

}


