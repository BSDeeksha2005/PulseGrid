#pragma once

#include <cstdint>
#include <mutex>
#include <vector>
#include <chrono>

#include "EventProcessor.h" // for ProcessingResult, Severity

// A point-in-time copy of all aggregated stats + stored anomalies.
// Returned by value so callers never hold a reference into
// ResultAggregator's internal state.
struct AggregatorSnapshot {
    std::uint64_t totalEventsProcessed = 0;
    std::uint64_t totalAnomalies = 0;
    std::uint64_t lowSeverityAnomalies = 0;
    std::uint64_t mediumSeverityAnomalies = 0;
    std::uint64_t highSeverityAnomalies = 0;
    std::vector<ProcessingResult> anomalies;

    // Sum of per-event processing latencies, in microseconds.
    // Raw sum is kept alongside the derived average so callers
    // can recompute or re-aggregate later if needed.
    std::uint64_t totalLatencyMicroseconds = 0;
    double averageLatencyMicroseconds = 0.0;
};

// Thread-safe shared result store. Multiple worker threads may call
// addResult() concurrently. This class creates and manages no
// threads of its own -- it is purely a synchronized data sink.
//
// Note: this class tracks per-event processing latency (a business
// metric collected from workers), but it does NOT time the overall
// application run -- that measurement belongs in main(), which is
// the only place that sees the full producer/worker thread lifecycle.
class ResultAggregator {
public:
    ResultAggregator() = default;

    ResultAggregator(const ResultAggregator&) = delete;
    ResultAggregator& operator=(const ResultAggregator&) = delete;

    // Called concurrently by any number of worker threads.
    // `latency` is the time spent inside processEvent() for this
    // one event, as measured by the calling worker.
    void addResult(const ProcessingResult& result, std::chrono::microseconds latency);

    // One locked pass over all state -- guarantees every field in
    // the returned snapshot reflects the same instant.
    AggregatorSnapshot getSnapshot() const;

    std::uint64_t getTotalEventsProcessed() const;
    std::uint64_t getTotalAnomalies() const;
    std::uint64_t getLowSeverityAnomalies() const;
    std::uint64_t getMediumSeverityAnomalies() const;
    std::uint64_t getHighSeverityAnomalies() const;
    double getAverageLatencyMicroseconds() const;

    std::vector<ProcessingResult> getAnomalies() const;

private:
    mutable std::mutex mutex_;

    std::uint64_t totalEventsProcessed_ = 0;
    std::uint64_t totalAnomalies_ = 0;
    std::uint64_t lowSeverityAnomalies_ = 0;
    std::uint64_t mediumSeverityAnomalies_ = 0;
    std::uint64_t highSeverityAnomalies_ = 0;
    std::uint64_t totalLatencyMicroseconds_ = 0;

    std::vector<ProcessingResult> anomalies_;
};