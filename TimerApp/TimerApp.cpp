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
#include <QSettings>





TimerApp::TimerApp(QSqlDatabase& db, QWidget* parent)
    : QMainWindow(parent), dataBase(db)
{
    ui = new UI;
    setCentralWidget(ui->setupUi(this));

    lastCheckedDate = QDate::currentDate();

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QApplication::windowIcon());
    trayIcon->setToolTip("Timer App");
    trayIcon->show();

    // Connections
    connect(ui->startButton, &QPushButton::clicked,
        this, &TimerApp::onStartClicked);

    connect(ui->stopButton, &QPushButton::clicked,
        this, &TimerApp::onStopClicked);

    connect(ui->togglePause, &QPushButton::clicked,
        this, &TimerApp::onTogglePauseClicked);

    connect(ui->trackButton, &QPushButton::clicked,
        this, &TimerApp::onTrackAppClicked);

    connect(ui->scheduleButton, &QPushButton::clicked,
        this, &TimerApp::onScheduleClicked);

    // Refresh open apps list
    QTimer* refreshApps = new QTimer(this);

    connect(refreshApps, &QTimer::timeout, this, [this]() {
        ui->openedApps->clear();

        for (const QString& app :
            ActiveWindowHelper::getAllOpenProcesses()) {
            ui->openedApps->addItem(app);
        }
        });

    refreshApps->start(3000);

    // Calendar
    markScheduledDays(
        ui->calendar->yearShown(),
        ui->calendar->monthShown()
    );

    connect(ui->calendar, &QCalendarWidget::currentPageChanged,
        this, [this](int year, int month) {
            markScheduledDays(year, month);
        });

    connect(ui->calendar, &QCalendarWidget::clicked,
        this, &TimerApp::showScheduleForDate);

    // Focus check
    QTimer* focusCheck = new QTimer(this);

    connect(focusCheck, &QTimer::timeout,
        this, &TimerApp::CheckActiveWindow);

    focusCheck->start(1000);

    // Daily reset
    QSettings settings;

    QString today =
        QDate::currentDate().toString(Qt::ISODate);

    QString lastReset =
        settings.value("lastDailyResetDate").toString();

    if (lastReset != today) {
        ScheduleDataAccessObject::resetDailyLimits(dataBase);
        settings.setValue("lastDailyResetDate", today);
    }
}

//special Days calendar;
void TimerApp::markScheduledDays(int year, int month) {
    QTextCharFormat normalFormat;
    QTextCharFormat scheduledFormat;

    scheduledFormat.setBackground(Qt::black);
    scheduledFormat.setForeground(Qt::white);
    scheduledFormat.setFontWeight(QFont::Bold);

    QDate firstDay(year, month, 1);
    if (!firstDay.isValid())
        return;

    for (int day = 1; day <= firstDay.daysInMonth(); ++day) {
        ui->calendar->setDateTextFormat(
            QDate(year, month, day), normalFormat);
    }
    for (int day = 1; day <= firstDay.daysInMonth(); ++day) {
        QDate date(year, month, day);
        int weekday = date.dayOfWeek();
        for (const auto& [appkey, timer] : appTimers) {
            Q_UNUSED(timer);
                
            int limit = ScheduleDataAccessObject::getLimit(dataBase, appkey, weekday);
            if (limit > 0) {
                ui->calendar->setDateTextFormat(date, scheduledFormat);
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
    for (int i = 0; i < ui->onTimerApps->count(); ++i) {
        appCombo->addItem(ui->onTimerApps->item(i)->text());
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

        ui->statusLabel->setText("Limit set for " + appKey +
            " on " + dayCombo->currentText() +
            " to " + QString::number(limitSeconds) + "s");
        markScheduledDays(
            ui->calendar->yearShown(),
            ui->calendar->monthShown()
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
    QListWidgetItem* item = ui->onTimerApps->currentItem();

    if (!item) {
        ui->statusLabel->setText("Select a tracked app first");
        return;
    }

    QString key = item->text().split(" > ").first();

    bool ok;
    int secs = ui->secondsInput->text().toInt(&ok);

    if (ok && secs > 0) {
        // Quick timer: does not modify scheduled remaining time.
        if (appTimers.contains(key)) {
            appTimers[key]->start(secs);

            ui->statusLabel->setText(
                "Quick timer started: " + key +
                " (" + QString::number(secs) + "s)");

            updateRemaining(secs);
        }
        else {
            ui->statusLabel->setText("No tracked app found for " + key);
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

                ui->statusLabel->setText(
                    "Routine timer started: " + key +
                    " (" + QString::number(limit) + "s)");

                updateRemaining(limit);
            }
            else {
                ui->statusLabel->setText("No tracked app found for " + key);
            }
        }
        else {
            ui->statusLabel->setText("No DB limit set for " + key);
        }
    }

    // Highlight today in calendar.
    QTextCharFormat startFmt;
    startFmt.setBackground(Qt::darkYellow);
    startFmt.setFontWeight(QFont::Bold);
    startFmt.setForeground(Qt::cyan);

    QDate todayDate = QDate::currentDate();
    ui->calendar->setDateTextFormat(todayDate, startFmt);
}




void TimerApp::onTogglePauseClicked()
{
    if (ui->togglePause->text() == "Pause") {
        for (auto& [name, t] : appTimers) {
            if (t->isRunning())
                t->pause();
        }

        ui->togglePause->setText("Resume");
    }
    else {
        ui->togglePause->setText("Pause");
        CheckActiveWindow();
    }
}

void TimerApp::updateRemaining(int seconds) {
    if (seconds > 0) {
        ui->timerDisplay->setText("Remaining: " + timeFormat(seconds));
        ui->timerDisplay->setStyleSheet("color: green;");
    }
    else if(seconds < 0){
        int overuse = -seconds;
        ui->timerDisplay->setText("Overuse: " + timeFormat(overuse));
        ui->timerDisplay->setStyleSheet("color: red; font-weight: bold;");
    }
    else { ui->timerDisplay->setStyleSheet("color: white; font-weight: bold;"); }
}


void TimerApp::onStopClicked() {
    for (auto& [name, t] : appTimers) {
        if (t->isScheduled()) {
            int remaining = t->getRemaining();
            int today = QDate::currentDate().dayOfWeek();

            ScheduleDataAccessObject::updateRemaining(
                dataBase, name, today, remaining);
        }

        t->stop();
    }

    ui->togglePause->setText("Pause");
    ui->statusLabel->setText("Stopped");
}

void TimerApp::CheckActiveWindow() {
    QString activeProcess = ActiveWindowHelper::getActiveProcessName();
    QString activeTitle = ActiveWindowHelper::getActiveWindowTitle();
    QString key = makeKey(activeProcess, activeTitle);

    bool foundActive = false;  

    
    QDate currentDate = QDate::currentDate();
    int today = currentDate.dayOfWeek();
    

    // Get today’s weekday (Qt: 1=Mon, 5=fri)

    if (today > 5) {
        for (auto& [name, timer] : appTimers) {
            if (timer->isScheduled()) {
                timer->pause();
            }
        }

        ui->statusLabel->setText("No scheduled timers on weekends");
        return;
    }



    
    //.

    for (auto& [name, t] : appTimers) {
        if (name == key) {
            
            
          
            
            int limit = ScheduleDataAccessObject::getLimit(dataBase, key, today);

            if (limit > 0) {
                // enforce daily countdown limit
                if (!t->isRunning() && ui->togglePause->text() != "Resume") {
                    t->resume();
                }
                
                ui->statusLabel->setText("Active with limit: " + key +
                    " (" + QString::number(limit) + "s)");
            }
            else {
                // no limit set, just resume
                if (ui->togglePause->text() != "Resume") {
                    t->resume();
                }
                ui->statusLabel->setText("Active (no limit): " + key);
            }
            foundActive = true;
        }
        else {
            t->pause();
        }
    }
    
    if (!foundActive) {
        ui->statusLabel->setText("Paused (no tracked app in focus)");
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
    QListWidgetItem* item = ui->openedApps->currentItem();
    if (!item) {
        ui->statusLabel->setText("Select an app first");
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

        for (int i = 0; i < ui->onTimerApps->count(); i++) {
            if (ui->onTimerApps->item(i)->text().startsWith(key)) {
                delete ui->onTimerApps->takeItem(i);
                break;
            }
        }

        ui->statusLabel->setText("Detracked " + key);
        ui->trackButton->setText("Track App");
        return;
    }
    
    // Track new app
    auto timer = new TimerLogic(dataBase,this);

    connect(timer, &TimerLogic::remainingChanged, this, &TimerApp::updateRemaining);

    connect(timer, &TimerLogic::remainingChanged, this, [=](int sec) {
        for (int i = 0; i < ui->onTimerApps->count(); i++) {
            if (ui->onTimerApps->item(i)->text().startsWith(key)) {
                if (sec >= 0) {
                    ui->onTimerApps->item(i)->setText(key);
                }
                else {
                    int overuse = -sec; // negative means overuse
                    ui->onTimerApps->item(i)->setText(key + " > Overuse: " + QString::number(overuse) + "s");
                    ui->onTimerApps->item(i)->setForeground(Qt::red); // highlight overuse
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
        ui->statusLabel->setText(key + " finished");
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
        ui->statusLabel->setText(key + " paused");
        });

    appTimers[key] = timer;
    ui->onTimerApps->addItem(key);
    ui->statusLabel->setText("Tracking " + key);
    ui->trackButton->setText("Detrack App");
}

void TimerApp::showUsageData() {
    QList<QVariantMap> usageRecords = UsageDataAccessObject::getAll(dataBase);
    ui->usageList->clear();
    for (const auto& row : usageRecords) {
        QString line = QString("%1 (%2 min overuse) on %3")
            .arg(row["app"].toString())
            .arg(row["overuse_min"].toInt())
            .arg(row["date"].toString());
        ui->usageList->addItem(line);
    }

}


