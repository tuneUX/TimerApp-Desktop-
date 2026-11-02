#pragma once
#include <unordered_map>
#include <QString>
#include <QDate>

class ScheduleManager {
public:
    void setLimit(const QString& appKey, int weekday, int seconds);
    int getLimit(const QString& appKey, int weekday) const;

private:
    // appKey → weekday → allowed seconds
    std::unordered_map<QString, std::unordered_map<int, int>> weeklyLimits;
};
