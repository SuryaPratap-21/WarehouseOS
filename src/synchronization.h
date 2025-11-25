#ifndef SYNCHRONIZATION_H
#define SYNCHRONIZATION_H

#include "utilities.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>

class StockBuffer {
private:
    int capacity;
    std::queue<int> items;
    mutable std::mutex mtx;
    std::condition_variable cv;
    int producer_count;
    int consumer_count;

public:
    StockBuffer(int cap, int prod_count, int cons_count)
        : capacity(cap), producer_count(prod_count), consumer_count(cons_count) {}

    void produce(int producer_id, int item_id) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return (int)items.size() < capacity; });

        items.push(item_id);
        std::cout << "[PRODUCE] Producer " << producer_id << " produced item " << item_id
                  << " | Buffer: " << items.size() << "/" << capacity << "\n";
        cv.notify_all();
    }

    int consume(int consumer_id) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this] { return !items.empty(); });

        int item = items.front();
        items.pop();
        std::cout << "[CONSUME] Consumer " << consumer_id << " consumed item " << item
                  << " | Buffer: " << items.size() << "/" << capacity << "\n";
        cv.notify_all();
        return item;
    }

    int getBufferSize() const {
        std::lock_guard<std::mutex> lock(mtx);
        return items.size();
    }
};

class Synchronization {
private:
    StockBuffer buffer;
    int num_producers;
    int num_consumers;

public:
    Synchronization(int buffer_size, int prod, int cons)
        : buffer(buffer_size, prod, cons), num_producers(prod), num_consumers(cons) {}

    void runProducerConsumer() {
        ConsoleUI::printSection("Stock Replenishment Coordination - Producer-Consumer");

        std::vector<std::thread> threads;

        for (int i = 1; i <= num_producers; ++i) {
            threads.push_back(std::thread([this, i]() {
                buffer.produce(i, i * 100);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        for (int i = 1; i <= num_consumers; ++i) {
            threads.push_back(std::thread([this, i]() {
                int item = buffer.consume(i);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }));
        }

        for (auto& thread : threads) {
            if (thread.joinable()) thread.join();
        }

        ConsoleUI::printSuccess("Stock coordination completed");
        std::cout << "Final Buffer Size: " << buffer.getBufferSize() << "\n";
    }
};

#endif
