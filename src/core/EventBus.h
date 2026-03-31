/*
    [SYSTEM DESIGN/HEADER]

    @file: system.h
    @author: Matthew Green
    @date: March 30,2026

    @brief Creates an observer system for safe threading 
*/

#pragma once
#include <string>
#include <functional>
#include <unordered_map>
#include <vector>
#include <mutex>

// An event carries a name and an optional string payload.
struct Event
{
    std::string name;
    std::string payload;
};

using EventHandler = std::function<void(const Event&)>;

class EventBus
{
public:
    static EventBus& getInstance();
    void subscribe(const std::string& eventName, EventHandler handler);

    // Fire an event to the system
    void publish(const Event& event);

    // Just for convenience, and another way to publish an event
    void publish(const std::string& name, const std::string& payload = "");

    EventBus(const EventBus&)            = delete;
    EventBus& operator=(const EventBus&) = delete;

private:
    EventBus() = default;

    std::unordered_map<std::string, std::vector<EventHandler>> handlers_;
    std::mutex mutex_;
};
