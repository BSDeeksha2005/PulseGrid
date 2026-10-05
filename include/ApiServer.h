#pragma once
#include "ResultAggregator.h"

// Starts a blocking HTTP server that serves the FINAL (already-complete)
// AggregatorSnapshot. This is intentionally batch-then-serve:
// call this AFTER producer + all workers have joined.
// It does not spawn its own thread and does not return until the
// server is stopped (Ctrl+C / process exit).
void runApiServer(const AggregatorSnapshot& snapshot, int port);