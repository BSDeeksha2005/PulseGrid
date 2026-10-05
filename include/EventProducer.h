// include/EventProducer.h
#pragma once
#include "ThreadSafeQueue.h"
#include "TrafficEvent.h"

void generateTrafficEvents(ThreadSafeQueue<TrafficEvent>& queue,
                            int numEvents,
                            int numWorkers);