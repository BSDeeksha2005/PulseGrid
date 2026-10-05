#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <string>
#include <cstdlib>

#include "ThreadSafeQueue.h"
#include "TrafficEvent.h"
#include "EventProducer.h"
#include "EventWorker.h"
#include "ResultAggregator.h"
#include "Logger.h"
#include "ApiServer.h"

int main(int argc, char* argv[]) {
    const int QUEUE_CAPACITY = 5;
    int NUM_WORKERS = 4;
    const int NUM_EVENTS = 100;

    // Allow worker count to be supplied from the command line.
    // Example: ./build/PulseGrid 8
    if (argc > 1) {
        try {
            NUM_WORKERS = std::stoi(argv[1]);

            if (NUM_WORKERS <= 0) {
                std::cerr << "Worker count must be greater than 0.\n";
                return 1;
            }
        } catch (const std::exception&) {
            std::cerr << "Invalid worker count.\n";
            return 1;
        }
    }

    ThreadSafeQueue<TrafficEvent> queue(QUEUE_CAPACITY);
    ResultAggregator aggregator;
    Logger logger;

    auto startTime = std::chrono::steady_clock::now();

    std::thread producerThread(
        generateTrafficEvents,
        std::ref(queue),
        NUM_EVENTS,
        NUM_WORKERS
    );

    std::vector<std::thread> workers;

    for (int i = 1; i <= NUM_WORKERS; ++i) {
        workers.emplace_back(
            runEventWorker,
            i,
            std::ref(queue),
            std::ref(aggregator),
            std::ref(logger)
        );
    }

    producerThread.join();

    for (auto& worker : workers) {
        worker.join();
    }

    auto endTime = std::chrono::steady_clock::now();

    auto processingTime =
        std::chrono::duration_cast<std::chrono::microseconds>(
            endTime - startTime
        ).count();

    AggregatorSnapshot snapshot = aggregator.getSnapshot();

    std::cout << "\n========== PulseGrid Summary ==========\n";

    std::cout << "Workers: "
              << NUM_WORKERS << "\n";

    std::cout << "Total events processed: "
              << snapshot.totalEventsProcessed << "\n";

    std::cout << "Total anomalies: "
              << snapshot.totalAnomalies << "\n";

    std::cout << "Low severity: "
              << snapshot.lowSeverityAnomalies << "\n";

    std::cout << "Medium severity: "
              << snapshot.mediumSeverityAnomalies << "\n";

    std::cout << "High severity: "
              << snapshot.highSeverityAnomalies << "\n";

    std::cout << "Processing time: "
              << processingTime
              << " microseconds\n";

    std::cout << "=======================================\n";

    // Use the PORT supplied by the deployment platform.
    // Fall back to 8081 for local development.
    int port = 8081;

    if (const char* envPort = std::getenv("PORT")) {
        try {
            port = std::stoi(envPort);

            if (port <= 0) {
                port = 8081;
            }
        } catch (const std::exception&) {
            port = 8081;
        }
    }

    std::cout << "Starting API server on port "
              << port << "\n";

    runApiServer(snapshot, port);

    return 0;
}
