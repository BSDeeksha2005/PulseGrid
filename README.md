# PulseGrid

### Concurrent Traffic Telemetry & Anomaly Processing Pipeline

PulseGrid is a C++17 traffic telemetry processing system designed around a bounded producer-consumer pipeline and configurable worker pool.

The system generates traffic events, processes them concurrently, classifies anomalous behavior by severity, aggregates results safely across worker threads, and exposes the processed data through a REST API. A Next.js dashboard consumes the API and presents the results through an operations-style monitoring interface.

The project focuses on practical systems programming concepts including concurrency, synchronization, bounded queues, worker pools, latency measurement, thread-safe aggregation, testing, and API integration.

---

## Overview

PulseGrid models a simplified traffic telemetry processing system.

Traffic events are produced and placed into a bounded thread-safe queue. A configurable pool of worker threads consumes these events and passes them through the anomaly-processing pipeline.

Each event is classified as normal or anomalous. Anomalies are assigned one of three severity levels:

- **Low**
- **Medium**
- **High**

Processed results are aggregated in a thread-safe component and exposed through a REST API. The frontend dashboard retrieves this data and provides a visual representation of the completed analysis.

### System Flow

```text
                    ┌─────────────────┐
                    │  Traffic Events │
                    └────────┬────────┘
                             │
                             ▼
                    ┌─────────────────┐
                    │ Event Producer  │
                    └────────┬────────┘
                             │
                             ▼
                ┌──────────────────────────┐
                │ Bounded Thread-Safe     │
                │ Queue<TrafficEvent>     │
                └────────────┬─────────────┘
                             │
                    ┌────────┴────────┐
                    │                 │
                    ▼                 ▼
              ┌──────────┐      ┌──────────┐
              │ Worker 1 │  ... │ Worker N │
              └────┬─────┘      └────┬─────┘
                   │                 │
                   └────────┬────────┘
                            ▼
                   ┌─────────────────┐
                   │ Event Processor │
                   └────────┬────────┘
                            │
                            ▼
                   ┌─────────────────┐
                   │ Result          │
                   │ Aggregator      │
                   └────────┬────────┘
                            │
                  ┌─────────┴─────────┐
                  │                   │
                  ▼                   ▼
             ┌──────────┐       ┌────────────┐
             │  Logger  │       │ REST API   │
             └──────────┘       └─────┬──────┘
                                      │
                                      ▼
                              ┌──────────────┐
                              │ Next.js      │
                              │ Dashboard    │
                              └──────────────┘