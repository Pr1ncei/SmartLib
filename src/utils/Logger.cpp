/*
    [SYSTEM DESIGN/HEADER]

    @file: Logger.h
    @author: Matthew Green and Prince Pamintuan
    @date: March 20,2026

    @brief Implementation of recording events during the program execution
*/

#include "Logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// Singleton access
Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

// Construction / Destruction
Logger::Logger()
{
    logFile_.open("opac_log.txt", std::ios::app);
    if (!logFile_.is_open())
        std::cerr << "[LOGGER] Could not open opac_log.txt for writing.\n";
}

Logger::~Logger()
{
    if (logFile_.is_open())
        logFile_.close();
}

// Public API
void Logger::log(LogLevel level, const std::string& message)
{
    std::lock_guard<std::mutex> lock(mutex_);

    std::string entry = "[" + currentTimestamp() + "] "
        + "[" + levelToString(level) + "] "
        + message;

    if (logFile_.is_open())
        logFile_ << entry << "\n";

    std::cout << entry << "\n";
}

// Helpers
std::string Logger::currentTimestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm     tm = *std::localtime(&now);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::levelToString(LogLevel level)
{
    switch (level)
    {
    case LogLevel::INFO:    return "INFO   ";
    case LogLevel::WARNING: return "WARNING";
    case LogLevel::ERROR_L: return "ERROR  ";
    default:                return "UNKNOWN";
    }
}