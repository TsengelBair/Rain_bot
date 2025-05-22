#ifndef RAINBOT_H
#define RAINBOT_H

#include <tgbot/tgbot.h>

#include "Db/IDataBase.h"

class RainBot
{
public:
    explicit RainBot(const std::string &bot_token, IDataBase &db);

    void start();
    void sendAlertToAllUsers();

private:
    void setupCommands();

private:
    TgBot::Bot m_bot;
    IDataBase& m_db;
};

#endif // RAINBOT_H
