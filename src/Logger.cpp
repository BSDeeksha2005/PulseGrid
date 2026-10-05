// src/Logger.cpp
#include "Logger.h"

#include <iostream>

void Logger::log(const std::string& line) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::cout << line << "\n";
}