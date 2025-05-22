#ifndef SQLITEDBHANDLER_H
#define SQLITEDBHANDLER_H

#include <string>
#include <sqlite3.h>

#include "IDataBase.h"

class SqliteDbHandler : public IDataBase
{
public:
   explicit SqliteDbHandler(const std::string &db_path);
    ~SqliteDbHandler();

    bool userExist(const int tg_id) const override;
    void addUser(const int tg_id) override;
    void removeUser(const int tg_id) override;
    std::list<int> getAllUsers() const override;

private:
    void openDb();
    bool createTables();
    bool executeQuery(const std::string& query);

private:
    sqlite3* m_db;
};

#endif // SQLITEDBHANDLER_H
