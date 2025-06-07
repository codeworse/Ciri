#include <chrono>
#include <atomic>
#include "metrics/counter_sum.h"
#include "metrics/counter_minmax.h"
#include "metrics/counter_basic.h"
#include <gtest/gtest.h>


using namespace std::chrono_literals;


TEST(MetricsTests, CounterSum) {
    ciri::metrics::CounterSum c;
    for (size_t i = 0; i < 100; ++i) {
        c.increase(10);
    }

    for (size_t i = 0; i < 100; ++i) {
        c.decrease(10);
    }

    EXPECT_EQ(c.get(), 0);
}

TEST(MetricsTests, CounterMax) {
    ciri::metrics::CounterMinMax<size_t> c(ciri::metrics::CounterMinMax<size_t>::Mode::Max);
    for (size_t i = 0; i <= 100; ++i) {
        c.set(i);
    }

    EXPECT_EQ(c.get(), 100);
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
        size_t sum = dummy.load(std::memory_order_relaxed);
        for (size_t i = 0; i < n; ++i) {
            threads[i].join();
        }
        threads.clear();
        auto finish = std::chrono::system_clock::now();
        auto duration = finish - start;
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms\n";
    }
    {
        ciri::metrics::CounterSum c(4);
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
        size_t sum = c.get();
        for (size_t i = 0; i < n; ++i) {
            threads[i].join();
        }
        auto finish = std::chrono::system_clock::now();
        auto duration = finish - start;
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms\n";
    }
}
