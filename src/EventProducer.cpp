// src/EventProducer.cpp
#include "EventProducer.h"
#include <random>
#include <vector>
#include <string>
#include <algorithm>

void generateTrafficEvents(ThreadSafeQueue<TrafficEvent>& queue,
                            int numEvents,
                            int numWorkers) {

    std::vector<std::string> vehicleIds;
    for (int i = 1; i <= 25; ++i) {
        vehicleIds.push_back("V-" + std::to_string(1000 + i));
    }

    std::vector<std::string> locationIds;
    for (int i = 1; i <= 12; ++i) {
        locationIds.push_back("R-" + std::to_string(i));
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_int_distribution<> vehiclePick(0, vehicleIds.size() - 1);
    std::uniform_int_distribution<> locationPick(0, locationIds.size() - 1);
    std::normal_distribution<> normalSpeed(50.0, 15.0);   // mean 50 km/h
    std::uniform_int_distribution<> anomalyChance(1, 100);
    std::uniform_int_distribution<> eventTypeRoll(1, 100);

    for (int i = 0; i < numEvents; ++i) {
        TrafficEvent event;
        event.vehicleId = vehicleIds[vehiclePick(gen)];
        event.locationId = locationIds[locationPick(gen)];
        event.timestamp = std::chrono::system_clock::now();

        // Mostly normal speeds, occasionally inject a low-speed anomaly
        if (anomalyChance(gen) <= 8) {
            event.speed = std::uniform_real_distribution<>(0.0, 5.0)(gen);
        } else {
            event.speed = std::max(0.0, normalSpeed(gen));
        }

        // Event type distribution: mostly routine
        int roll = eventTypeRoll(gen);
        if (roll <= 90) {
            event.eventType = EventType::SpeedUpdate;
        } else if (roll <= 96) {
            event.eventType = EventType::SuddenStop;
        } else {
            event.eventType = EventType::IncidentReport;
        }

        queue.push(event);
    }

    // Poison pills — one per worker, using sentinel vehicleId
    for (int i = 0; i < numWorkers; ++i) {
        TrafficEvent shutdownSignal;
        shutdownSignal.vehicleId = "__SHUTDOWN__";
        shutdownSignal.locationId = "";
        shutdownSignal.timestamp = std::chrono::system_clock::now();
        shutdownSignal.speed = 0.0;
        shutdownSignal.eventType = EventType::SpeedUpdate;
        queue.push(shutdownSignal);
    }
}