/*
    [SYSTEM DESIGN/HEADER]

    @file: auth.h
    @author: Jaz Angeles, Prince Pamintuan, and Matthew Green
    @date: March 20,2026

    @brief The authentication system of SmartLib
*/

#pragma once
#include <string>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include "../../db/Database.h"
#include "../../models/Account.h"

class AuthService
{
public:
    // Returns an Account with the user's role, or an empty Account on failure.
    Account login(const std::string& username, const std::string& password);

    // Registers a new user. Returns false if username already exists.
    bool registerUser(const std::string& username,
                      const std::string& password,
                      const std::string& role = "user");

    bool userExists(const std::string& username);

private:
    Database db_;
};
