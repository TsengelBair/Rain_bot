#include <boost/beast.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "requesthandler.h"
#include "Config.h"
#include "Logger/logger.h"

namespace http = boost::beast::http;

nlohmann::json RequestHandler::getWeather()
{
    Logger& logger = Logger::getInstance();
    logger.log(LogLevel::INFO, "Начало получения данных о погоде");

    boost::asio::io_context ioc;
    boost::asio::ip::tcp::resolver resolver(ioc);
    boost::asio::ip::tcp::socket socket(ioc);

    try {
        logger.log(LogLevel::DEBUG, "Попытка подключения к " + host);

        auto results = resolver.resolve(host, "80");
        boost::asio::connect(socket, results);

        logger.log(LogLevel::INFO, "Успешное подключение к " + host);

        http::request<http::string_body> req(http::verb::get, target, 11);
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        logger.log(LogLevel::DEBUG, "Отправка запроса: " + target);
        http::write(socket, req);

        boost::beast::flat_buffer buffer;
        http::response<http::dynamic_body> res;

        logger.log(LogLevel::DEBUG, "Ожидание ответа от сервера");
        http::read(socket, buffer, res);

        logger.log(LogLevel::DEBUG, "Попытка парсинга JSON ответа");
        auto jsonResponse = nlohmann::json::parse(boost::beast::buffers_to_string(res.body().data()));

        if (jsonResponse.empty()) {
            logger.log(LogLevel::WARNING, "Получен пустой JSON ответ");
        } else {
            logger.log(LogLevel::INFO, "Успешно получены данные о погоде");
        }

        socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        socket.close();

        return jsonResponse;

    } catch (const nlohmann::json::parse_error& e) {
        logger.log(LogLevel::ERROR,
            "Ошибка парсинга JSON: " + std::string(e.what()) +
            " (id: " + std::to_string(e.id) + ")");
        return nlohmann::json{};
    } catch (const boost::system::system_error& e) {
        logger.log(LogLevel::ERROR,
            "Системная ошибка: " + std::string(e.what()) +
            " (код: " + std::to_string(e.code().value()) + ")");
        return nlohmann::json{};
    } catch (const std::exception& e) {
        logger.log(LogLevel::ERROR, "Ошибка: " + std::string(e.what()));
        return nlohmann::json{};
    } catch (...) {
        logger.log(LogLevel::CRITICAL, "Неизвестная ошибка при получении погоды");
        return nlohmann::json{};
    }
}

