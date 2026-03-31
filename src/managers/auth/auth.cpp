/*
    [SYSTEM IMPLEMENTATION]

    @file: auth.cpp
    @author: Jaz Angeles, Prince Pamintuan, and Matthew Green
    @date: March 20,2026

    @brief Implements the authentication system of SmartLib
*/

#include "auth.h"
#include "../../utils/Logger.h"
#include "../../core/EventBus.h"
#include <iostream>
#include <cppconn/prepared_statement.h>

// WARNING: The problem of this implementation is that it is unsecured.
// The passwords are stored as plain text and anyone wiht the database access
// can read all the passwords 
// For future development, we suggest adding a password hashing like bcrypt 

// It starts by establishing a connection for the SQL Database
// Inputs the credentials and checks if it exists in the database with query
// If successful, it checks the role (admin or user) to show the necessary screen
// If not, create an instance that will show LOGIN FAIL and logs it
Account AuthService::login(const std::string& username,
                           const std::string& password)
{
    try
    {
        sql::Connection* con = db_.getConnection();

        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT role FROM users WHERE username=? AND password=?"));
        pstmt->setString(1, username);
        pstmt->setString(2, password);

        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        if (res->next())
        {
            std::string role = res->getString("role");
            Logger::getInstance().info("LOGIN OK - user: " + username +
                                       "  role: " + role);

            EventBus::getInstance().publish("LOGIN_SUCCESS",
                                            "username=" + username +
                                            ";role=" + role);
            return Account(username, password, role);
        }

        Logger::getInstance().warning("LOGIN FAIL - user: " + username);
        EventBus::getInstance().publish("LOGIN_FAILED", "username=" + username);
        return Account();
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(std::string("[AuthService::login] ") +
                                    e.what());
        return Account();
    }
}

// It starts by establishing a connection for the SQL Database
// Inputs the details for registration and add it to the database
// It only has Username, Password, and Confirm Password during the create account screen
// If error occurs, it usually an error in the database or the connection itself
bool AuthService::registerUser(const std::string& username,
                                const std::string& password,
                                const std::string& role)
{
    try
    {
        if (userExists(username))
        {
            Logger::getInstance().warning(
                "REGISTER FAIL - username taken: " + username);
            return false;
        }

        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "INSERT INTO users (username, password, role) VALUES (?,?,?)"));
        pstmt->setString(1, username);
        pstmt->setString(2, password);
        pstmt->setString(3, role);
        pstmt->execute();

        Logger::getInstance().info("REGISTER OK - user: " + username +
                                   "  role: " + role);
        EventBus::getInstance().publish("USER_REGISTERED",
                                        "username=" + username);
        return true;
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[AuthService::registerUser] ") + e.what());
        return false;
    }
}

// This is used for Logging in to the system by querying if it exists in the database 
bool AuthService::userExists(const std::string& username)
{
    try
    {
        sql::Connection* con = db_.getConnection();
        std::unique_ptr<sql::PreparedStatement> pstmt(
            con->prepareStatement(
                "SELECT 1 FROM users WHERE username=?"));
        pstmt->setString(1, username);
        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());
        return res->next();
    }
    catch (std::exception& e)
    {
        Logger::getInstance().error(
            std::string("[AuthService::userExists] ") + e.what());
        return false;
    }
}
