// src/EventProcessor.cpp
#include "EventProcessor.h"

ProcessingResult processEvent(const TrafficEvent& event) {
    ProcessingResult result;
    result.sourceEvent = event;

    if (event.eventType == EventType::IncidentReport) {
        result.isAnomaly = true;
        result.reason = "Reported incident";
        result.severity = Severity::High;
        return result;
    }

    if (event.eventType == EventType::SuddenStop) {
        result.isAnomaly = true;
        result.reason = "Sudden stop detected";
        result.severity = Severity::Medium;
        return result;
    }

    if (event.speed < 5.0) {
        result.isAnomaly = true;
        result.reason = "Low speed - possible congestion or stoppage";
        result.severity = Severity::Low;
        return result;
    }

    result.isAnomaly = false;
    result.reason = "Normal";
    result.severity = Severity::Low;
    return result;
}