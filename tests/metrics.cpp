#include <chrono>
#include <atomic>
#include "metrics/counter.h"
#include <gtest/gtest.h>


using namespace std::chrono_literals;

struct DummyCounter {
    std::atomic_size_t counter{0};
};
TEST(MetricsTests, CounterBasics) {
    ciri::metrics::Counter<size_t> c(2);
    for (size_t i = 0; i < 100; ++i) {
        c.increase(10);
    }

    for (size_t i = 0; i < 100; ++i) {
        c.decrease(10);
    }

    EXPECT_EQ(c.get(), 0);
}
TEST(MetricsTests, CounterStress) {
    size_t n = 4;
    size_t iterations = 100'000'000;
    std::vector<std::thread> threads;
    threads.reserve(n);
    {
        std::atomic_size_t dummy;
        std::cout << "dummy: ";
        auto start = std::chrono::system_clock::now();
        for (size_t i = 0; i < n; ++i) {
            threads.emplace_back([&]() {
                size_t sum = 0;
                for (size_t k = 0; k < iterations; ++k) {
                    dummy.fetch_add(k, std::memory_order_relaxed);
                }
            });
        }
        for (size_t i = 0; i < n; ++i) {
            threads[i].join();
        }
        threads.clear();
        auto finish = std::chrono::system_clock::now();
        auto duration = finish - start;
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms\n";
    }
    {
        ciri::metrics::Counter<size_t> c(4);
        std::cout << "ciri: ";
        auto start = std::chrono::system_clock::now();
        for (size_t i = 0; i < n; ++i) {
            threads.emplace_back([&]() {
                size_t sum = 0;
                for (size_t k = 0; k < iterations; ++k) {
                    c.increase(k);
                }
            });
        }
        for (size_t i = 0; i < n; ++i) {
            threads[i].join();
        }
        auto finish = std::chrono::system_clock::now();
        auto duration = finish - start;
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms\n";
    }
}
