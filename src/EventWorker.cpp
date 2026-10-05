// src/EventWorker.cpp

#include "EventWorker.h"
#include <sstream>
#include <string>
#include <chrono>

namespace {

    const std::string SHUTDOWN_SIGNAL = "__SHUTDOWN__";

    std::string severityToString(Severity severity) {
        switch (severity) {
            case Severity::Low:    return "LOW";
            case Severity::Medium: return "MEDIUM";
            case Severity::High:   return "HIGH";
            default:               return "UNKNOWN";
        }
    }

}

void runEventWorker(
    int workerId,
    ThreadSafeQueue<TrafficEvent>& queue,
    ResultAggregator& aggregator,
    Logger& logger
) {
    while (true) {

        TrafficEvent event = queue.pop();

        if (event.vehicleId == SHUTDOWN_SIGNAL) {
            std::ostringstream oss;
            oss << "Worker " << workerId << " shutting down";
            logger.log(oss.str());
            break;
        }

        const auto processingStart = std::chrono::steady_clock::now();
        ProcessingResult result = processEvent(event);
        const auto processingEnd = std::chrono::steady_clock::now();

        const auto latency = std::chrono::duration_cast<std::chrono::microseconds>(
            processingEnd - processingStart
        );

        aggregator.addResult(result, latency);

        if (result.isAnomaly) {

            std::ostringstream oss;
            oss << "Worker " << workerId
                << " | ANOMALY"
                << " | vehicle=" << event.vehicleId
                << " | road=" << event.locationId
                << " | reason=" << result.reason
                << " | severity=" << severityToString(result.severity)
                << " | latency_us=" << latency.count();
            logger.log(oss.str());

        } else {

            std::ostringstream oss;
            oss << "[Worker " << workerId
                << "] Event processed normally | vehicle="
                << event.vehicleId
                << " | road=" << event.locationId
                << " | speed=" << event.speed
                << " | latency_us=" << latency.count();
            logger.log(oss.str());
        }
    }
}