#pragma once
#include <string>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include "../../db/Database.h"

class AuthService
{
private: 
	Database db; 

public:
	std::string login(std::string username, std::string password);
	bool registerUser(std::string username, std::string password, std::string role);

	bool userExists(std::string username);

};

