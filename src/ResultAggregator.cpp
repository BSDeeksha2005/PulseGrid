#include "ResultAggregator.h"

void ResultAggregator::addResult(const ProcessingResult& result, std::chrono::microseconds latency) {
    std::lock_guard<std::mutex> lock(mutex_);

    ++totalEventsProcessed_;
    totalLatencyMicroseconds_ += static_cast<std::uint64_t>(latency.count());

    if (result.isAnomaly) {
        ++totalAnomalies_;

        switch (result.severity) {
            case Severity::Low:
                ++lowSeverityAnomalies_;
                break;
            case Severity::Medium:
                ++mediumSeverityAnomalies_;
                break;
            case Severity::High:
                ++highSeverityAnomalies_;
                break;
        }

        anomalies_.push_back(result);
    }
}

AggregatorSnapshot ResultAggregator::getSnapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);

    AggregatorSnapshot snapshot;
    snapshot.totalEventsProcessed = totalEventsProcessed_;
    snapshot.totalAnomalies = totalAnomalies_;
    snapshot.lowSeverityAnomalies = lowSeverityAnomalies_;
    snapshot.mediumSeverityAnomalies = mediumSeverityAnomalies_;
    snapshot.highSeverityAnomalies = highSeverityAnomalies_;
    snapshot.anomalies = anomalies_; // copied while locked
    snapshot.totalLatencyMicroseconds = totalLatencyMicroseconds_;

    snapshot.averageLatencyMicroseconds =
        totalEventsProcessed_ > 0
            ? static_cast<double>(totalLatencyMicroseconds_) / static_cast<double>(totalEventsProcessed_)
            : 0.0;

    return snapshot;
}

std::uint64_t ResultAggregator::getTotalEventsProcessed() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalEventsProcessed_;
}

std::uint64_t ResultAggregator::getTotalAnomalies() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalAnomalies_;
}

std::uint64_t ResultAggregator::getLowSeverityAnomalies() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return lowSeverityAnomalies_;
}

std::uint64_t ResultAggregator::getMediumSeverityAnomalies() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return mediumSeverityAnomalies_;
}

std::uint64_t ResultAggregator::getHighSeverityAnomalies() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return highSeverityAnomalies_;
}

double ResultAggregator::getAverageLatencyMicroseconds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return totalEventsProcessed_ > 0
        ? static_cast<double>(totalLatencyMicroseconds_) / static_cast<double>(totalEventsProcessed_)
        : 0.0;
}

std::vector<ProcessingResult> ResultAggregator::getAnomalies() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return anomalies_;
}