#pragma once
#include <QObject>
#include <QTimer>
#include <chrono>

class TimerLogic : public QObject {
    Q_OBJECT
public:
    explicit TimerLogic(QObject* parent = nullptr);

    void start(int durationSec);
    void stop();
    void pause();
    void resume();

    int getRemaining() const;
    int getDuration() const;

signals:
    void remainingChanged(int seconds);
    void finished();
    void paused();

private slots:
    void tick();

private:
    QTimer tickTimer;
    int duration = 0;
    int pauseRemaining = 0;
    bool running = false;
    std::chrono::steady_clock::time_point endTime;
};
