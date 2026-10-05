// include/EventProcessor.h
#pragma once
#include "TrafficEvent.h"
#include <string>

enum class Severity {
    Low,
    Medium,
    High
};

struct ProcessingResult {
    bool isAnomaly;
    std::string reason;
    Severity severity;
    TrafficEvent sourceEvent;
};

ProcessingResult processEvent(const TrafficEvent& event);