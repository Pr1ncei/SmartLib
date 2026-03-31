/*
    [SYSTEM DESIGN/HEADER]

    @file: database.h
    @author: Prince Pamintuan
    @date: March 11,2026

    @brief Wrapper for MySQL Database using MySQL Connector C++ Library 
*/

#pragma once
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/exception.h>
#include <mysql_driver.h>
#include <stdexcept>
#include <iostream>
#include "../utils/env.hpp"

// Singleton Connection to the Database 
// For now and only for school project purposes, 
// this runs locally in the system 
class Database
{
public:
    // Implemented the Rule of 5 (Well, 4 since Move Constructor are unnecessary for now)
    // That consists of Constructor, Destructor, Copy Constructor, and Copy Assignment Operator
    Database();
    ~Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;

    sql::Connection* getConnection();

private:
    sql::mysql::MySQL_Driver* driver_ = nullptr;
    sql::Connection* con_ = nullptr;

    void connect();
};