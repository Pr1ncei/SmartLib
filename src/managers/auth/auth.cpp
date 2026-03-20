#include "auth.h"

std::string AuthService::login(std::string username, std::string password)
{
	sql::Connection* con = db.getConnection();
	sql::Statement* stmt = con->createStatement();

	sql::ResultSet* result = stmt->executeQuery(
		"SELECT * FROM users WHERE username='" + username +
		"' AND password='" + password + "'"
	);

	std::string role = "";

	if (result->next())
	{
		role = result->getString("role");
	}

	delete result; 
	delete stmt; 

	return role; 
}

bool AuthService::registerUser(std::string username, 
									  std::string password, 
									  std::string role)
{
	try
	{
		if (userExists(username))
			return false;

		sql::Connection* con = db.getConnection();
		sql::Statement* stmt = con->createStatement();

		stmt->execute(
			"INSERT INTO users (username, password, role) VALUES ('" +
			username + "', '" + password + "', '" + role + "')"
		);

		delete stmt;
		return true;
	}
	catch (std::exception& e)
	{
		std::cout << "[AUTH.CPP] Database Error: " << e.what() << std::endl;
		return false;
	}
}

bool AuthService::userExists(std::string username)
{
	sql::Connection* con = db.getConnection();
	sql::Statement* stmt = con->createStatement();
	sql::ResultSet* result = stmt->executeQuery(
		"SELECT 1 FROM users WHERE username='" + username + "'"
	);

	bool exists = result->next();

	delete result; 
	delete stmt; 

	return exists; 
}