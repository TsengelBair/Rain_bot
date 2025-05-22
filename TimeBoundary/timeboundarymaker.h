#ifndef TIMEBOUNDARYMAKER_H
#define TIMEBOUNDARYMAKER_H

#include <iostream>
#include <ctime>

class TimeBoundaryMaker
{
public:
    TimeBoundaryMaker();
    std::pair<std::time_t, std::time_t> getBoundaries() const;

private:
    std::time_t getCurrentGmtUnixTime();
    std::time_t resetTimeToMidnight(std::time_t timeInUnix);

private:
    std::time_t m_currentTime;
    std::time_t m_leftBoundary;
    std::time_t m_rightBoundary;
};

#endif // TIMEBOUNDARYMAKER_H
