/*
    [SYSTEM IMPLEMENTATION]

    @file: EventBus.cpp
    @author: Matthew Green
    @date: March 30,2026

    @brief Creates an observer system for safe threading 
*/

#include "EventBus.h"

EventBus& EventBus::getInstance()
{
    static EventBus instance;
    return instance;
}

// Similar to Youtube's subscribe feature, it's basically
// listening for new events that happened and run it
// This can have multiple instances of this function 
void EventBus::subscribe(const std::string& eventName, EventHandler handler)
{
    std::lock_guard<std::mutex> lock(mutex_);
    handlers_[eventName].push_back(std::move(handler));
}

void EventBus::publish(const Event& event)
{
    // Copy handlers under lock to avoid holding the lock during callbacks
    std::vector<EventHandler> toCall;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = handlers_.find(event.name);
        if (it != handlers_.end())
            toCall = it->second;
    }

    for (const auto& handler : toCall)
        handler(event);
}

void EventBus::publish(const std::string& name, const std::string& payload)
{
    publish(Event{name, payload});
}
