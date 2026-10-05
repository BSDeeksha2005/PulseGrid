#include <gtest/gtest.h>

#include "EventProcessor.h"

namespace {

TrafficEvent makeEvent(EventType type, double speed) {
    TrafficEvent event;
    event.vehicleId = "V-TEST";
    event.locationId = "R-TEST";
    event.speed = speed;
    event.eventType = type;
    event.timestamp = std::chrono::system_clock::now();
    return event;
}

}

TEST(EventProcessorTests, NormalSpeedIsNotAnomaly) {
    TrafficEvent event = makeEvent(EventType::SpeedUpdate, 50.0);

    ProcessingResult result = processEvent(event);

    EXPECT_FALSE(result.isAnomaly);
    EXPECT_EQ(result.reason, "Normal");
    EXPECT_EQ(result.severity, Severity::Low);
}

TEST(EventProcessorTests, LowSpeedIsLowSeverityAnomaly) {
    TrafficEvent event = makeEvent(EventType::SpeedUpdate, 3.0);

    ProcessingResult result = processEvent(event);

    EXPECT_TRUE(result.isAnomaly);
    EXPECT_EQ(result.reason, "Low speed - possible congestion or stoppage");
    EXPECT_EQ(result.severity, Severity::Low);
}

TEST(EventProcessorTests, SuddenStopIsMediumSeverityAnomaly) {
    TrafficEvent event = makeEvent(EventType::SuddenStop, 60.0);

    ProcessingResult result = processEvent(event);

    EXPECT_TRUE(result.isAnomaly);
    EXPECT_EQ(result.reason, "Sudden stop detected");
    EXPECT_EQ(result.severity, Severity::Medium);
}

TEST(EventProcessorTests, IncidentReportIsHighSeverityAnomaly) {
    TrafficEvent event = makeEvent(EventType::IncidentReport, 40.0);

    ProcessingResult result = processEvent(event);

    EXPECT_TRUE(result.isAnomaly);
    EXPECT_EQ(result.reason, "Reported incident");
    EXPECT_EQ(result.severity, Severity::High);
}
