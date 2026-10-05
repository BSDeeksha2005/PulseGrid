#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class ThreadSafeQueue {
public:
    explicit ThreadSafeQueue(size_t maxCapacity)
        : capacity(maxCapacity) {}

    void push(T item) {
        std::unique_lock<std::mutex> lock(mtx);
        notFull.wait(lock, [this] {
            return queue.size() < capacity;
        });
        queue.push(std::move(item));
        notEmpty.notify_one();
    }

    T pop() {
        std::unique_lock<std::mutex> lock(mtx);
        notEmpty.wait(lock, [this] {
            return !queue.empty();
        });
        T item = std::move(queue.front());
        queue.pop();
        notFull.notify_one();
        return item;
    }

private:
    std::queue<T> queue;
    std::mutex mtx;
    std::condition_variable notFull;
    std::condition_variable notEmpty;
    size_t capacity;
};