#pragma once
#include <string>
#include <chrono>

enum class EventType {
    SpeedUpdate,
    SuddenStop,
    IncidentReport
};

struct TrafficEvent {
    std::string vehicleId;
    std::string locationId;
    std::chrono::system_clock::time_point timestamp;
    double speed;
    EventType eventType;
};