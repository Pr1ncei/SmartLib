/*
    [SYSTEM DESIGN/HEADER]

    @file: Logger.h
    @author: Matthew Green and Prince Pamintuan
    @date: March 20,2026

    @brief Records events during the program execution
*/

#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <ctime>

// We made this along side with the Event Bus to make it easier
// to debug and see all of the events that are going on while the 
// GUI application is running

// For now, we have three which is for:
// info - like editing books
// warning - two instances happening
// error_l - can happen when there's a missing file that is required like env
enum class LogLevel { INFO, WARNING, ERROR_L };

class Logger
{
public:
    static Logger& getInstance();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(LogLevel level, const std::string& message);

    void info(const std::string& msg) { log(LogLevel::INFO, msg); }
    void warning(const std::string& msg) { log(LogLevel::WARNING, msg); }
    void error(const std::string& msg) { log(LogLevel::ERROR_L, msg); }

private:
    Logger();                 // Opens log file.
    ~Logger();                // Closes log file.

    std::ofstream logFile_;
    std::mutex    mutex_;

    static std::string currentTimestamp();
    static std::string levelToString(LogLevel level);
};