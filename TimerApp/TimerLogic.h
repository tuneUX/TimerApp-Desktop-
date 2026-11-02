#pragma once

#include <QObject>
#include <QTimer>
#include <QSqlDatabase>
#include <chrono>

class TimerLogic : public QObject {
    Q_OBJECT

public:
    explicit TimerLogic(QSqlDatabase& db, QObject* parent = nullptr);

    void start(int durationSec);          // Custom timer
    void startScheduled(int remainingSec); // Scheduled timer

    void stop();
    void pause();
    void resume();

    bool isRunning() const;
    bool isScheduled() const;
    int getRemaining() const;
    int getOveruseMinutes() const;

signals:
    void remainingChanged(int seconds);
    void finished();
    void paused();

private slots:
    void tick();

private:
    void startInternal(int durationSec, bool scheduled);

    QSqlDatabase& dataBase;
    int allowedSeconds = 0;
    int elapsedSeconds = 0;
    QTimer tickTimer;
    int duration = 0;
    int pauseRemaining = 0;
    bool running = false;
    bool scheduledMode = false;
    bool finishedEmitted = false;

    std::chrono::steady_clock::time_point endTime;
};