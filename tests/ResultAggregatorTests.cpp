#include <gtest/gtest.h>

#include "ResultAggregator.h"

namespace {

ProcessingResult makeResult(
    bool anomaly,
    Severity severity,
    const std::string& vehicleId
) {
    ProcessingResult result;

    result.isAnomaly = anomaly;
    result.severity = severity;
    result.reason = anomaly ? "Test anomaly" : "Normal";

    result.sourceEvent.vehicleId = vehicleId;
    result.sourceEvent.locationId = "R-TEST";
    result.sourceEvent.speed = 50.0;
    result.sourceEvent.eventType = EventType::SpeedUpdate;
    result.sourceEvent.timestamp = std::chrono::system_clock::now();

    return result;
}

}

TEST(ResultAggregatorTests, CountsEventsAndAnomaliesCorrectly) {
    ResultAggregator aggregator;

    aggregator.addResult(makeResult(false, Severity::Low, "V-1"), std::chrono::microseconds(100));
    aggregator.addResult(makeResult(true, Severity::Low, "V-2"), std::chrono::microseconds(100));
    aggregator.addResult(makeResult(true, Severity::Medium, "V-3"), std::chrono::microseconds(100));
    aggregator.addResult(makeResult(true, Severity::High, "V-4"), std::chrono::microseconds(100));

    AggregatorSnapshot snapshot = aggregator.getSnapshot();

    EXPECT_EQ(snapshot.totalEventsProcessed, 4);
    EXPECT_EQ(snapshot.totalAnomalies, 3);

    EXPECT_EQ(snapshot.lowSeverityAnomalies, 1);
    EXPECT_EQ(snapshot.mediumSeverityAnomalies, 1);
    EXPECT_EQ(snapshot.highSeverityAnomalies, 1);
}

TEST(ResultAggregatorTests, StoresOnlyAnomalies) {
    ResultAggregator aggregator;

    aggregator.addResult(makeResult(false, Severity::Low, "V-1"), std::chrono::microseconds(100));
    aggregator.addResult(makeResult(true, Severity::Medium, "V-2"), std::chrono::microseconds(100));
    aggregator.addResult(makeResult(true, Severity::High, "V-3"), std::chrono::microseconds(100));

    AggregatorSnapshot snapshot = aggregator.getSnapshot();

    ASSERT_EQ(snapshot.anomalies.size(), 2);

    EXPECT_EQ(snapshot.anomalies[0].sourceEvent.vehicleId, "V-2");
    EXPECT_EQ(snapshot.anomalies[1].sourceEvent.vehicleId, "V-3");
}

TEST(ResultAggregatorTests, EmptyAggregatorHasZeroCounts) {
    ResultAggregator aggregator;

    AggregatorSnapshot snapshot = aggregator.getSnapshot();

    EXPECT_EQ(snapshot.totalEventsProcessed, 0);
    EXPECT_EQ(snapshot.totalAnomalies, 0);
    EXPECT_EQ(snapshot.lowSeverityAnomalies, 0);
    EXPECT_EQ(snapshot.mediumSeverityAnomalies, 0);
    EXPECT_EQ(snapshot.highSeverityAnomalies, 0);
    EXPECT_TRUE(snapshot.anomalies.empty());
}
