#include <iostream>
#include "sqlitedbhandler.h"
#include "Logger/logger.h"

SqliteDbHandler::SqliteDbHandler(const std::string &db_path) : m_db(nullptr)
{
    Logger::getInstance().log(LogLevel::INFO, "Создание объекта sqlite db, попытка открыть бд");
    openDb();
}

SqliteDbHandler::~SqliteDbHandler()
{
    if (m_db) {
        sqlite3_close(m_db);
        Logger::getInstance().log(LogLevel::INFO, "Закрытие приложения, вызываю декструктор sqlite db");
    }
}

bool SqliteDbHandler::userExist(const int tg_id) const
{
    std::string query = "SELECT COUNT(*) FROM users WHERE tg_id = " + std::to_string(tg_id) + ";";

    /// любой sql запрос (в виде строки) преобразуется в объект stmt в sqlite (это подготовленный запрос)
    sqlite3_stmt* stmt;
    /*
        1 - указатель на бд
        2 - указатель на строку запроса
        3 - длина строки (-1 означает, что неизвестно, sqlite сам определит)
        4 - указатель на подготовленный запрос
        5 - доп параметры (0 - значит не используются)
    */
    int rc = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        Logger::getInstance().log(LogLevel::ERROR, "Ошибка при при подготовке запроса на существование пользователя: "
                                                         + std::to_string(tg_id) + std::string(sqlite3_errmsg(m_db)));
        return false;
    }

    /// выполняем запрос - функция вернет статус запроса
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return count > 0;  /// Если count > 0, то пользователь существует
    }

    /// освобождение ресурсов
    sqlite3_finalize(stmt);
    return false;
}

void SqliteDbHandler::addUser(const int tg_id)
{
    Logger::getInstance().log(LogLevel::INFO, "Попытка добавить пользователя: " + std::to_string(tg_id));

    std::string query = "INSERT INTO users (tg_id) VALUES (" + std::to_string(tg_id) + ");";
    if (executeQuery(query)) {
        Logger::getInstance().log(LogLevel::INFO, "Пользователь: " + std::to_string(tg_id) + " добавлен");
    } else {
        Logger::getInstance().log(LogLevel::ERROR, "Ошибка при попытке добавить пользователя: " + std::to_string(tg_id));
    }
}

void SqliteDbHandler::removeUser(const int tg_id)
{
    Logger::getInstance().log(LogLevel::INFO, "Попытка удалить пользователя: " + std::to_string(tg_id));

    std::string query = "DELETE FROM users WHERE tg_id = " + std::to_string(tg_id) + ";";
    if (executeQuery(query)) {
        Logger::getInstance().log(LogLevel::INFO, "Пользователь: " + std::to_string(tg_id) + " удален");
    } else {
        Logger::getInstance().log(LogLevel::ERROR, "Ошибка при попытке удалить пользователя: " + std::to_string(tg_id));
    }
}

std::list<int> SqliteDbHandler::getAllUsers() const
{
    std::list<int> users;

    std::string query = "SELECT tg_id FROM users;";
    sqlite3_stmt* stmt;

    int rc = sqlite3_prepare_v2(m_db, query.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        Logger::getInstance().log(LogLevel::ERROR, "Ошибка при подготовке запроса на получение всех пользователей: "
                                                                               + std::string(sqlite3_errmsg(m_db)));
        return users; /// пустой список в случае ошибки
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int userId = sqlite3_column_int(stmt, 0);
        users.push_back(userId);
    }

    sqlite3_finalize(stmt);
    return users;
}

void SqliteDbHandler::openDb()
{
    int rc = sqlite3_open("tg_users.db", &m_db);
    if (rc) {
        Logger::getInstance().log(LogLevel::ERROR, "Не получилось открыть sqite db: "
                                              + std::string(sqlite3_errmsg(m_db)));
    } else {
        Logger::getInstance().log(LogLevel::INFO, "Успешное открытие sqlite db");
        bool ok = createTables();
        if (ok) {
            Logger::getInstance().log(LogLevel::INFO, "Успешное создание таблиц после открытия бд");
        } else {
            Logger::getInstance().log(LogLevel::ERROR, "Не удалось создать таблицы");
        }
    }
}

bool SqliteDbHandler::createTables()
{
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            tg_id INTEGER NOT NULL UNIQUE
        );
    )";

    char* errMsg;
    int rc = sqlite3_exec(m_db, sql, nullptr, 0, &errMsg);
    if (rc != SQLITE_OK) {
        Logger::getInstance().log(LogLevel::ERROR, "Ошибка при создании таблиц: " + std::string(errMsg));

        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool SqliteDbHandler::executeQuery(const std::string& query)
{
    char* errMsg = nullptr;
    int result = sqlite3_exec(m_db, query.c_str(), 0, 0, &errMsg);

    if (result != SQLITE_OK){
        Logger::getInstance().log(LogLevel::ERROR, "Ошибка при выполнении sql запроса: " + std::string(errMsg));

        sqlite3_free(errMsg);
        return false;
    }

    return true;
}
