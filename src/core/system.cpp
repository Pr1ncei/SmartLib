/*
    [SYSTEM IMPLEMENTATION]

    @file: system.cpp
    @author: Matthew Green
    @date: March 11,2026

    @brief Acts as a controller for the whole SmartLib Application
*/

#include "System.h"
#include "../ui/main_window.h"
#include "../utils/Logger.h"
#include "EventBus.h"
#include <QApplication>
#include <chrono>

System::System() : running_(false)
{
    Logger::getInstance().info("=== OPAC System starting ===");
}

System::~System()
{
    stopAuditThread();
    Logger::getInstance().info("=== OPAC System stopped ===");
}

// Runs/Initialize the QT Application and displays the window 
// Just like how int main() works, it also uses argc and argv 
// for it work. This is just a way to avoid CLI arguments. 
void System::run()
{
    startAuditThread();
    int    argc = 0;
    char** argv = nullptr;
    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    app.exec(); 

    stopAuditThread();
}


void System::startAuditThread()
{
    running_.store(true);
    auditThread_ = std::thread(&System::auditLoop, std::ref(running_));
    Logger::getInstance().info("Audit thread started.");
}

void System::stopAuditThread()
{
    if (running_.load())
    {
        running_.store(false);
        if (auditThread_.joinable())
            auditThread_.join();
        Logger::getInstance().info("Audit thread stopped.");
    }
}

// Listens for events in the background to log it
// Example, if the user tries to borrow or return the book, it observes it and logs it down.
// We're using this to debug any problems that may happen during the application
void System::auditLoop(std::atomic<bool>& running)
{
    EventBus::getInstance().subscribe("BOOK_BORROWED", [](const Event& e) {
        Logger::getInstance().info("[AUDIT] Borrow recorded - " + e.payload);
        });
    EventBus::getInstance().subscribe("BOOK_RETURNED", [](const Event& e) {
        Logger::getInstance().info("[AUDIT] Return recorded - " + e.payload);
        });

    while (running.load())
    {
        for (int i = 0; i < 60 && running.load(); ++i)
            std::this_thread::sleep_for(std::chrono::seconds(1));
        if (running.load())
            Logger::getInstance().info("[AUDIT] Heartbeat - system is running.");
    }
}