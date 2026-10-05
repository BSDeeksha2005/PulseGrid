cmake_minimum_required(VERSION 3.16)

project(PulseGrid CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

include(FetchContent)

FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.18.3
)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/archive/refs/tags/v3.11.3.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(
    httplib
    json
    googletest
)

add_executable(PulseGrid
    src/main.cpp
    src/EventProducer.cpp
    src/EventWorker.cpp
    src/EventProcessor.cpp
    src/ResultAggregator.cpp
    src/Logger.cpp
    src/ApiServer.cpp
)

target_include_directories(PulseGrid PRIVATE include)

target_compile_options(PulseGrid PRIVATE
    -Wall
    -Wextra
)

target_link_libraries(PulseGrid PRIVATE
    Threads::Threads
    httplib::httplib
    nlohmann_json::nlohmann_json
)

enable_testing()

add_executable(PulseGridTests
    tests/EventProcessorTests.cpp
    tests/ResultAggregatorTests.cpp
    tests/ThreadSafeQueueTests.cpp
    src/EventProcessor.cpp
    src/ResultAggregator.cpp
)

target_include_directories(PulseGridTests PRIVATE include)

target_compile_options(PulseGridTests PRIVATE
    -Wall
    -Wextra
)

target_link_libraries(PulseGridTests PRIVATE
    GTest::gtest_main
    Threads::Threads
)

add_test(
    NAME PulseGridTests
    COMMAND PulseGridTests
)