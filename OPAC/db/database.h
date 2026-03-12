#ifndef DATABASE_H
#define DATABASE_H

#include <mysql_driver.h>
#include <mysql_connection.h>

class Database {
private:
    sql::Connection* con;

public:
    Database();
    ~Database();
    sql::Connection* getConnection();
};

#endif