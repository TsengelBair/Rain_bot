#include <chrono>

#include "timeboundarymaker.h"

TimeBoundaryMaker::TimeBoundaryMaker()
{
    m_currentTime = getCurrentGmtUnixTime();
    std::time_t midnight = resetTimeToMidnight(m_currentTime);

    /// В формате GMT: 03:00, 21:00
    m_leftBoundary = midnight + 6 * 3600;
    m_rightBoundary = midnight + 24 * 3600;
}

std::pair<time_t, time_t> TimeBoundaryMaker::getBoundaries() const
{
    return std::make_pair(m_leftBoundary, m_rightBoundary);
}

time_t TimeBoundaryMaker::getCurrentGmtUnixTime()
{
    auto now = std::chrono::system_clock::now();

    std::time_t unixTime = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    return unixTime;
}

time_t TimeBoundaryMaker::resetTimeToMidnight(time_t timeInUnix)
{
    /// преобразуем в структуру даты, чтобы сбросить часы минуты секунды (т.о. полночь)
    tm* timeStruct = localtime(&timeInUnix);

    timeStruct->tm_sec = 0;
    timeStruct->tm_min = 0;
    timeStruct->tm_hour = 0;

    time_t curDateMidnight = mktime(timeStruct);
    return curDateMidnight;
}
