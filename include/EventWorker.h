#pragma once

#include "ThreadSafeQueue.h"
#include "TrafficEvent.h"
#include "EventProcessor.h"
#include "ResultAggregator.h"
#include "Logger.h"

void runEventWorker(
    int workerId,
    ThreadSafeQueue<TrafficEvent>& queue,
    ResultAggregator& aggregator,
    Logger& logger
);