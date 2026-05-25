#include "TimerLogic.h"

TimerLogic::TimerLogic(QObject* parent) : QObject(parent) {
    connect(&tickTimer, &QTimer::timeout, this, &TimerLogic::tick);
}

void TimerLogic::start(int durationSec) {
    duration = durationSec;
    endTime = std::chrono::steady_clock::now() + std::chrono::seconds(durationSec);
    tickTimer.start(1000); // tick every second
    running = true;
    pauseRemaining = durationSec;
}

void TimerLogic::stop() {
    tickTimer.stop();
    running = false;
    pauseRemaining = 0;
    emit remainingChanged(0);
}

void TimerLogic::pause() {
    if (!running) return;
    tickTimer.stop();
    running = false;
    pauseRemaining = std::chrono::duration_cast<std::chrono::seconds>(
        endTime - std::chrono::steady_clock::now()).count();

    emit remainingChanged(pauseRemaining);  // freeze UI at current number
    emit paused();
}

void TimerLogic::resume() {
    if (!running && pauseRemaining > 0) {
        endTime = std::chrono::steady_clock::now() + std::chrono::seconds(pauseRemaining);
        tickTimer.start(1000);
        running = true;
    }
}

void TimerLogic::tick() {
    if (!running) return;
    auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
        endTime - std::chrono::steady_clock::now()).count();

    if (remaining > 0) {
        pauseRemaining = static_cast<int>(remaining);
        emit remainingChanged(pauseRemaining);
    }
    else {
        tickTimer.stop();
        running = false;
        emit remainingChanged(0);
        emit finished();
    }
}

int TimerLogic::getRemaining() const {
    if (running) {
        return std::chrono::duration_cast<std::chrono::seconds>(
            endTime - std::chrono::steady_clock::now()).count();
    }
    return pauseRemaining;
}

int TimerLogic::getDuration() const {
    return duration;
}
