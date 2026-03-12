#include "database.h"
#include <iostream>

Database::Database() {
    try {
        sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://127.0.0.1:3306", "root", "");
        con->setSchema("opacdb");
    }
    catch (sql::SQLException& e) {
        std::cerr << "DB Connection Error: " << e.what() << std::endl;
        con = nullptr;
    }
}

Database::~Database() {
    if (con) {
        delete con;
    }
}

sql::Connection* Database::getConnection() {
    return con;
}