#include "menu.h"

void Menu::mainMenu()
{
	int choice; 

	do
	{
        std::cout << "\n=================================\n";
        std::cout << "        LIBRARY SYSTEM\n";
        std::cout << "=================================\n";
        std::cout << "[1] Login\n";
        std::cout << "[2] Register\n";
        std::cout << "[3] Exit\n";
        std::cout << "---------------------------------\n";
        std::cout << "Enter choice: ";
	
        std::cin >> choice; 

        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            std::cout << "Invalid input!\n";
            continue;
        }

        switch (choice)
        {
        case 1: loginMenu(); break; 
        case 2: registerMenu(); break; 
        case 3: std::cout << "Exiting system..\n"; break;
        default: std::cout << "Invalid choice.\n";
        }
    
    } while (choice != 3);

}

void Menu::loginMenu()
{
    std::string username, password; 

    std::cout << "\n========== LOGIN ==========\n";
    std::cout << "Username (0 to cancel): ";
    std::cin >> username;

    if (username == "0") return; 

    std::cout << "Password (0 to cancel): ";
    std::cin >> password;

    if (password == "0") return; 

    AuthService auth; 
    std::string role = auth.login(username, password);

    if (!role.empty())
    {
        std::cout << "\nWelcome " << username << "\n";

        if (role == "admin")
            adminMenu(username);
        else
            userMenu(username);
    }
    else
    {
        std::cout << "Invalid login.\n";
    }
}

void Menu::registerMenu()
{
    std::string username, password, confirm; 
    AuthService auth; 

    std::cout << "\n========== REGISTER ==========\n";
    while (true)
    {
        std::cout << "Enter Username (0 to cancel): ";
        std::cin >> username;

        if (username == "0") return;

        if (username.empty())
        {
            std::cout << "Username cannot be empty.\n";
            continue;
        }

        if (auth.userExists(username))
        {
            std::cout << "Username already exists.\n";
            continue;
        }

        break;
    }

    while (true)
    {
        std::cout << "Enter Password (0 to cancel): ";
        std::cin >> password;

        if (password == "0") return;

        std::cout << "Confirm Password: ";
        std::cin >> confirm;

        if (password != confirm)
        {
            std::cout << "Passwords do not match.\n";
            continue;
        }

        break;
    }

    if (auth.registerUser(username, password, "user"))
    {
        std::cout << "Account created successfully!\n";
    } 
    else
    {
        std::cout << "Something went wrong. Please try again.\n";
    }

}