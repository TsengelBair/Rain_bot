#ifndef IDATABASE_H
#define IDATABASE_H

#include <list>

/// Абстрактный класс - т.к. реализацию могу поменять на postgres
class IDataBase
{
public:
    virtual ~IDataBase(){};

    virtual bool userExist(const int tg_id) const = 0;
    virtual void addUser(const int tg_id) = 0;
    virtual void removeUser(const int tg_id) = 0;
    virtual std::list<int> getAllUsers() const = 0;
};

#endif // IDATABASE_H
