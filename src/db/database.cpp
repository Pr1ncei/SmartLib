/*
    [SYSTEM IMPLEMENTATION]

    @file: database.cpp
    @author: Prince Pamintuan
    @date: March 11,2026

    @brief Impleneting the wrapper for MySQL Database using MySQL Connector C++ Library
*/

#include "Database.h"
#include "../utils/Logger.h"
#include <cppconn/exception.h>
#include <mysql_driver.h>
#include <mysql_connection.h>

Database::Database()
{
    connect();
}

Database::~Database()
{
    if (con_)
    {
        delete con_;
        con_ = nullptr;
    }
}

sql::Connection* Database::getConnection()
{
    if (!con_)
        throw std::runtime_error("[Database] No active database connection.");
    return con_;
}

// It starts by reading the provided .env file, which is located at the root directory
// Sets each line to connect to the Local SQL Server with the following order: Server, Username, Password
// Once complete, it creates an instance of the database and logs it
// Otherwise, it will stop the program from continuing due to connection failed
// Do note that you must know your MySQL Credentials for the application to work. I failed in this so many times
void Database::connect()
{
    auto env = loadEnv(".env");

    const std::string server = env.count("DB_SERVER") ? env["DB_SERVER"] : "tcp://127.0.0.1:3306"; // tcp 127.0.0.1:3306 is usually the default localhost address 
    const std::string username = env.count("DB_USERNAME") ? env["DB_USERNAME"] : "";
    const std::string password = env.count("DB_PASSWORD") ? env["DB_PASSWORD"] : "";

    try
    {
        driver_ = sql::mysql::get_mysql_driver_instance();
        con_ = driver_->connect(server, username, password);
        con_->setSchema("opac_db");

        Logger::getInstance().info("Connected to MySQL - schema: opac_db");
    }
    catch (sql::SQLException& e)
    {
        Logger::getInstance().error(
            std::string("[Database] Connection failed: ") + e.what() +
            " (code=" + std::to_string(e.getErrorCode()) + ")");
        con_ = nullptr;
        throw std::runtime_error(
            std::string("[Database] Connection failed: ") + e.what());
    }
}