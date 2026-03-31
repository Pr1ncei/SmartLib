/*
    [SYSTEM IMPLEMENTATION]

    @file: env.hpp
    @author: Prince Pamintuan
    @date: March 19,2026

    @brief Custom file reader made for specifcally for .env files
*/

#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

// Since there is no custom env file reader just like in Python
// We decided to create one and use it to avoid putting hardcoded passwords in the source code
// the env file must be placed in the root directory and
// it is also required to connect to the sql database and asked during the installation

// For implementation, we just create a map to store all of its contents
// then, we loaded the file and check if it can access/open it 
// If successful, it will start a loop to read each line and 
// store key,value pair in the map 
// If not, it will stop the application and outputs that it cannot be open
inline std::unordered_map<std::string, std::string> loadEnv(
    const std::string& filename)
{
    std::unordered_map<std::string, std::string> env;
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cerr << "[ENV.HPP] Cannot open: " << filename << "\n";
        return env;
    }

    std::string line;
    while (std::getline(file, line))
    {
        if (!line.empty() && line.back() == '\r')
            line.pop_back();

        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        auto trim = [](std::string& s) {
            s.erase(0, s.find_first_not_of(" \t"));
            s.erase(s.find_last_not_of(" \t") + 1);
            };
        trim(key);
        trim(value);

        env[key] = value;
    }

    return env;
}