#include "ApiServer.h"

#include <httplib.h>
#include <nlohmann/json.hpp>

#include <ctime>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

namespace {

std::string severityToString(Severity s) {
    switch (s) {
        case Severity::Low:    return "Low";
        case Severity::Medium: return "Medium";
        case Severity::High:   return "High";
    }
    return "Unknown";
}

std::string eventTypeToString(EventType t) {
    switch (t) {
        case EventType::SpeedUpdate:    return "SpeedUpdate";
        case EventType::SuddenStop:     return "SuddenStop";
        case EventType::IncidentReport: return "IncidentReport";
    }
    return "Unknown";
}

std::string toIso8601(const std::chrono::system_clock::time_point& tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm utcTm{};
#if defined(_WIN32)
    gmtime_s(&utcTm, &t);
#else
    gmtime_r(&t, &utcTm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&utcTm, "%Y-%m-%dT%H:%M:%S") << "Z";
    return oss.str();
}

json toJson(const TrafficEvent& e) {
    return json{
        {"vehicleId",  e.vehicleId},
        {"locationId", e.locationId},
        {"timestamp",  toIso8601(e.timestamp)},
        {"speed",      e.speed},
        {"eventType",  eventTypeToString(e.eventType)}
    };
}

json toJson(const ProcessingResult& r) {
    return json{
        {"isAnomaly",   r.isAnomaly},
        {"reason",      r.reason},
        {"severity",    severityToString(r.severity)},
        {"sourceEvent", toJson(r.sourceEvent)}
    };
}

json summaryToJson(const AggregatorSnapshot& snapshot) {
    return json{
        {"totalEventsProcessed",     snapshot.totalEventsProcessed},
        {"totalAnomalies",           snapshot.totalAnomalies},
        {"lowSeverityAnomalies",     snapshot.lowSeverityAnomalies},
        {"mediumSeverityAnomalies",  snapshot.mediumSeverityAnomalies},
        {"highSeverityAnomalies",    snapshot.highSeverityAnomalies}
    };
}

json anomaliesToJson(const AggregatorSnapshot& snapshot) {
    json arr = json::array();
    for (const auto& result : snapshot.anomalies) {
        arr.push_back(toJson(result));
    }
    return arr;
}

} // namespace

void runApiServer(const AggregatorSnapshot& snapshot, int port) {
    httplib::Server server;

    server.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });
    
    // Snapshot is frozen (batch-then-serve), so it's safe to capture by
    // value into each handler with no additional synchronization.
    server.Get("/api/summary", [snapshot](const httplib::Request&, httplib::Response& res) {
        res.set_content(summaryToJson(snapshot).dump(), "application/json");
    });

    server.Get("/api/anomalies", [snapshot](const httplib::Request&, httplib::Response& res) {
        res.set_content(anomaliesToJson(snapshot).dump(), "application/json");
    });

    server.listen("0.0.0.0", port);
}
