#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>

enum LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

class Logger
{
public:
    static Logger& getInstance();
    void log(LogLevel level, const std::string &message);

private:
    std::ofstream m_logFile;

    Logger();
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator = (const Logger&) = delete;

private:
    std::string getCurrentDateTime();
    std::string levelToString(LogLevel level);
};

#endif // LOGGER_H
