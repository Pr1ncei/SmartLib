#pragma once
#include <string>
#include <iostream>
#include "../managers/auth/auth.h"

class Menu
{
	void mainMenu();
	void adminMenu(std::string username);
	void userMenu(std::string username);

	void loginMenu();
	void registerMenu();
};

