#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <ctime>
#include <nlohmann/json.hpp>

class Parser
{
public:
    Parser(const std::pair<std::time_t, std::time_t> &boundaries);
    bool isRain(const nlohmann::json& json);

private:
    std::pair<std::time_t, std::time_t> m_boundaries;
};

#endif // PARSER_H
