#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>

#include "logger.h"

Logger::Logger() : m_logFile("output.log", std::ios::app) /// ios::app - добавляет инфу в файл без перезаписи
{
    if (!m_logFile.is_open()) {
        std::cerr << "Ошибка, открытия файла журнала: " << std::endl;
    }
}

Logger::~Logger()
{
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
}

Logger &Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::log(LogLevel level, const std::string &message)
{
    if (!m_logFile.is_open()) {
        std::cerr << "Ошибка, открытия файла журнала: " << std::endl;
    }

    /// сформированная строка лога
    m_logFile << getCurrentDateTime() << " [" << levelToString(level) << "] " << message << std::endl;
}

std::string Logger::getCurrentDateTime()
{
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm* now_tm = std::localtime(&now_c);

    std::ostringstream oss;
    oss << std::put_time(now_tm, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}

std::string Logger::levelToString(LogLevel level)
{
    switch (level) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}
