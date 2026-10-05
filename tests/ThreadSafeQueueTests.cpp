#include <gtest/gtest.h>

#include "ThreadSafeQueue.h"

#include <chrono>
#include <future>
#include <string>

using namespace std::chrono_literals;

TEST(ThreadSafeQueueTests, PushAndPopPreserveFIFOOrder) {
    ThreadSafeQueue<int> queue(3);

    queue.push(10);
    queue.push(20);
    queue.push(30);

    EXPECT_EQ(queue.pop(), 10);
    EXPECT_EQ(queue.pop(), 20);
    EXPECT_EQ(queue.pop(), 30);
}

TEST(ThreadSafeQueueTests, QueueBlocksWhenCapacityIsFull) {
    ThreadSafeQueue<int> queue(1);

    queue.push(1);

    auto producer = std::async(std::launch::async, [&queue]() {
        queue.push(2);
        return true;
    });

    EXPECT_EQ(
        producer.wait_for(200ms),
        std::future_status::timeout
    );

    EXPECT_EQ(queue.pop(), 1);

    EXPECT_EQ(
        producer.wait_for(200ms),
        std::future_status::ready
    );

    EXPECT_TRUE(producer.get());

    EXPECT_EQ(queue.pop(), 2);
}

TEST(ThreadSafeQueueTests, QueueSupportsMultipleItems) {
    ThreadSafeQueue<std::string> queue(5);

    queue.push("first");
    queue.push("second");
    queue.push("third");

    EXPECT_EQ(queue.pop(), "first");
    EXPECT_EQ(queue.pop(), "second");
    EXPECT_EQ(queue.pop(), "third");
}
