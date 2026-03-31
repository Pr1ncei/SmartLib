/*
    [SYSTEM DESIGN/HEADER] 

    @file: system.h
    @author: Matthew Green
    @date: March 11,2026

    @brief Acts as a controller for the whole SmartLib Application
*/

#pragma once
#include <string>
#include <thread>
#include <atomic>

class System
{
public:
    // Creates instance of the application and destructs when stopped
    System();
    ~System();

    // Start the application
    void run();

private:
    // Parallel Programming 
    std::thread      auditThread_;
    std::atomic<bool> running_;

    void startAuditThread();
    void stopAuditThread();
    static void auditLoop(std::atomic<bool>& running);
};