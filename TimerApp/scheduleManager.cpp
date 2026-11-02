#include "ScheduleManager.h"

void ScheduleManager::setLimit(const QString& appKey, int weekday, int seconds) {
    weeklyLimits[appKey][weekday] = seconds;
}

int ScheduleManager::getLimit(const QString& appKey, int weekday) const {
    auto appIt = weeklyLimits.find(appKey);
    if (appIt != weeklyLimits.end()) {
        auto dayIt = appIt->second.find(weekday);
        if (dayIt != appIt->second.end()) {
            return dayIt->second;
        }
    }
    return -1; // no limit set
}
