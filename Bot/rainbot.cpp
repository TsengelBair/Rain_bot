#include "rainbot.h"
#include "Logger/logger.h"

RainBot::RainBot(const std::string &bot_token, IDataBase &db) : m_bot(bot_token), m_db(db)
{
    setupCommands();
}

void RainBot::sendAlertToAllUsers()
{
    std::list<int> users = m_db.getAllUsers();
    if (users.empty()) {
        Logger::getInstance().log(LogLevel::INFO, "Список пользователей для рассылки пуст");
    }

    Logger::getInstance().log(LogLevel::INFO, "Рассылаю уведомление");
    for (const auto& userId : users) {
        m_bot.getApi().sendMessage(userId, "Ожидается дождь");
    }
}

void RainBot::start()
{
    try {
        Logger::getInstance().log(LogLevel::INFO, "Бот запущен в режиме long-poll");
        TgBot::TgLongPoll longPoll(m_bot);

        while (true) {
            longPoll.start();
        }

    } catch (const TgBot::TgException& e) {
        Logger::getInstance().log(LogLevel::ERROR,
            "Ошибка в работе бота: " + std::string(e.what()));
    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::CRITICAL,
            "Неизвестная ошибка: " + std::string(e.what()));
    } catch (...) {
        Logger::getInstance().log(LogLevel::CRITICAL,
            "Неизвестное исключение в работе бота");
    }
}

void RainBot::setupCommands()
{
    m_bot.getEvents().onCommand("start", [&](TgBot::Message::Ptr message) {
        m_bot.getApi().sendMessage(message->chat->id,
        "Привет! Я отправляю подписанным пользователям предупреждения о дожде,"
        " чтобы подписаться на уведомления, отправьте команду /subscribe ");
    });

    m_bot.getEvents().onCommand("subscribe", [&](TgBot::Message::Ptr message) {
        if (m_db.userExist(message->chat->id)) {
            m_bot.getApi().sendMessage(message->chat->id, "Вы уже подписаны на уведомления");
        } else {
            m_db.addUser(message->chat->id);
            m_bot.getApi().sendMessage(message->chat->id, "Вы подписались на уведомления,"
                                                          " для того, чтобы отписаться, отправьте команду /reject");
        }
    });

    m_bot.getEvents().onCommand("reject", [&](TgBot::Message::Ptr message) {
        if (!m_db.userExist(message->chat->id)) {
            m_bot.getApi().sendMessage(message->chat->id, "Вы не были подписаны, желаете подписаться? /subscribe");
        } else {
            m_db.removeUser(message->chat->id);
            m_bot.getApi().sendMessage(message->chat->id, "Уведомления больше не будут приходить,"
                                                          " для того чтобы снова получать уведомления, отправьте /subscribe");
        }
    });
}
