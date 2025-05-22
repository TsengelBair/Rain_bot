#include <nlohmann/json.hpp>

#include "scheduler.h"
#include "Bot/rainbot.h"
#include "Logger/logger.h"
#include "Request/requesthandler.h"
#include "TimeBoundary/timeboundarymaker.h"
#include "Parser/parser.h"

Scheduler::Scheduler(RainBot &bot) : m_bot(bot), m_isRunning(false)
{
    Logger::getInstance().log(LogLevel::INFO, "Планировщик создан");
}

Scheduler::~Scheduler()
{
    Logger::getInstance().log(LogLevel::INFO, "Вызов деструктора планировщика");
    stop();
}

void Scheduler::start()
{
    if (m_isRunning) {
        Logger::getInstance().log(LogLevel::INFO, "Попытка запустить уже работающий планировщик");
        return;
    }

    m_isRunning = true;
    try {
        m_thread = std::thread(&Scheduler::runInOtherThread, this);
        Logger::getInstance().log(LogLevel::INFO, "Планировщик успешно запущен в отдельном потоке");
    } catch (const std::exception &e) {
        m_isRunning = false;
        Logger::getInstance().log(LogLevel::ERROR, "Ошибка при запуске потока планировщика: " + std::string(e.what()));
    }

}

void Scheduler::runInOtherThread()
{
    Logger::getInstance().log(LogLevel::INFO, "Поток планировщика начал работу");

    try {
        while (m_isRunning) {
            time_t curDateAndTime = time(0);
            tm* timeStruct = localtime(&curDateAndTime);

            if (timeStruct->tm_hour == 5) {
                if (isRainExpected()) {
                    Logger::getInstance().log(LogLevel::INFO, "Обнаружен дождь");
                    m_bot.sendAlertToAllUsers();
                }
                /// чтобы избежать повторной отправки
                std::this_thread::sleep_for(std::chrono::minutes(60));
            }
            /// проверку диапазона времени осуществляем раз в 30 минут, чтобы точно попасть в диапазон 5 часов
            std::this_thread::sleep_for(std::chrono::minutes(30));
        }
    } catch (const std::exception &e) {

    }
}

bool Scheduler::isRainExpected()
{
    /// Запрос и ответ в виде json
    nlohmann::json jsonResponse = RequestHandler::getWeather();

    /// Диапазон времени (стартовая и конечная граница) проверки дождя
    TimeBoundaryMaker maker;
    std::pair<std::time_t, std::time_t> boundaries = maker.getBoundaries();

    /// Создаем парсер json ответа, передав в конструктор диапазон (ранее созданные границы) проверки
    Parser parser(boundaries);
    bool rain = parser.isRain(jsonResponse);

    /// Логируем результат запроса о дожде
    logRainRequestResult(boundaries, rain);
    return rain;
}

void Scheduler::logRainRequestResult(const std::pair<time_t, time_t> &boundaries, bool isRain)
{
    std::time_t startBoundary = boundaries.first;
    std::time_t endBoundary = boundaries.second;

    /// Преобразуем границы времени в структуру tm
    std::tm startTm = *std::localtime(&startBoundary);
    std::tm endTm = *std::localtime(&endBoundary);

    std::ostringstream oss;
    oss << "Диапазон времени: "
        << std::put_time(&startTm, "%Y-%m-%d %H:%M:%S") << " - "
        << std::put_time(&endTm, "%Y-%m-%d %H:%M:%S")
        << " | Наличие дождя: " << (isRain ? "Да" : "Нет");

    /// Логируем диапазон времени
    Logger::getInstance().log(LogLevel::INFO, oss.str());
}

void Scheduler::stop()
{
    Logger::getInstance().log(LogLevel::INFO, "Запущена остановка планировщика");
    m_isRunning = false;

    try {
        if (m_thread.joinable()) {
            Logger::getInstance().log(LogLevel::DEBUG, "Поток планировщика joinable, выполняю join");
            m_thread.join();
            Logger::getInstance().log(LogLevel::INFO, "Поток планировщика успешно остановлен");
        } else {
            Logger::getInstance().log(LogLevel::WARNING,
                "Попытка остановить неjoinable поток (уже завершён или никогда не запускался)");
        }
    } catch (const std::system_error& e) {
        Logger::getInstance().log(LogLevel::ERROR,
            "Ошибка при join потока: " + std::string(e.what()) + " (код: " + std::to_string(e.code().value()) + ")");
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::CRITICAL,
            "Критическая ошибка при остановке потока: " + std::string(e.what()));
    } catch (...) {
        Logger::getInstance().log(LogLevel::CRITICAL,
            "Неизвестная ошибка при остановке потока");
    }
}

