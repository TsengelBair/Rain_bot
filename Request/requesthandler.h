#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <string>
#include <nlohmann/json.hpp>

class RequestHandler
{
public:
    static nlohmann::json getWeather();

    /// Удаляем конструкторы и операторы присваивания, т.к. статический класс
    RequestHandler() = delete;
    ~RequestHandler() = delete;
    RequestHandler(const RequestHandler&) = delete;
    RequestHandler& operator=(const RequestHandler&) = delete;
    RequestHandler(RequestHandler&&) = delete;
    RequestHandler& operator=(RequestHandler&&) = delete;

};

#endif // REQUESTHANDLER_H
