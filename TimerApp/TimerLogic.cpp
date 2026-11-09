#include "TimerLogic.h"
#include "UsageDataAccessObject.h"

#include <QDateTime>
#include <QDebug>
#include <algorithm>

TimerLogic::TimerLogic(QSqlDatabase& db, QObject* parent)
    : QObject(parent), dataBase(db)
{
    connect(&tickTimer, &QTimer::timeout,
        this, &TimerLogic::tick);
}

void TimerLogic::start(int durationSec)
{
    startInternal(durationSec, false);
}

void TimerLogic::startScheduled(int remainingSec)
{
    startInternal(remainingSec, true);
}

void TimerLogic::startInternal(int durationSec, bool scheduled)
{
    if (durationSec <= 0)
        return;

    tickTimer.stop();

    duration = durationSec;
    allowedSeconds = durationSec;
    elapsedSeconds = 0;
    pauseRemaining = durationSec;
    scheduledMode = scheduled;
    finishedEmitted = false;

    endTime = std::chrono::steady_clock::now()
        + std::chrono::seconds(durationSec);

    running = true;
    tickTimer.start(1000);

    emit remainingChanged(durationSec);
}

bool TimerLogic::isRunning() const
{
    return running;
}

bool TimerLogic::isScheduled() const
{
    return scheduledMode;
}

void TimerLogic::stop()
{
    tickTimer.stop();
    running = false;
    pauseRemaining = 0;
    elapsedSeconds = 0;
    finishedEmitted = false;

    emit stopped();
}

void TimerLogic::pause()
{
    if (!running)
        return;

    pauseRemaining = std::max(0, getRemaining());

    tickTimer.stop();
    running = false;

    emit remainingChanged(pauseRemaining);
    emit paused();
}

void TimerLogic::resume()
{
    if (running || pauseRemaining <= 0)
        return;

    endTime = std::chrono::steady_clock::now()
        + std::chrono::seconds(pauseRemaining);

    running = true;
    tickTimer.start(1000);
}

int TimerLogic::getRemaining() const
{
    if (running) {
        return std::max(0, static_cast<int>(
            std::chrono::duration_cast<std::chrono::seconds>(
                endTime - std::chrono::steady_clock::now()).count()));
    }

    return pauseRemaining;
}

int TimerLogic::getOveruseMinutes() const
{
    const int overuseSeconds =
        std::max(0, elapsedSeconds - allowedSeconds);

    return overuseSeconds / 60;
}
// tick every second function
void TimerLogic::tick()
{
    if (!running)
        return;

    const auto now = std::chrono::steady_clock::now();

    int remaining = static_cast<int>(
        std::chrono::duration_cast<std::chrono::seconds>(
            endTime - now).count());

    if (remaining > 0) {
        pauseRemaining = remaining;
        elapsedSeconds = duration - remaining;
        emit remainingChanged(remaining);
        return;
    }

    // Countdown reached zero.
    elapsedSeconds = duration;
    pauseRemaining = 0;
    emit remainingChanged(0);

    if (!finishedEmitted) {
        finishedEmitted = true;
        emit finished();
    }

    // Continue counting overuse after the allowed time.
    const int overuseSeconds = static_cast<int>(
        std::chrono::duration_cast<std::chrono::seconds>(
            now - endTime).count());

    elapsedSeconds = duration + std::max(0, overuseSeconds);

    if (overuseSeconds > 0) {
        emit remainingChanged(-overuseSeconds);
    }
}