#include <ciri/metrics/CounterMinMax.h>
#include <ciri/metrics/CounterSum.h>
#include <ciri/utils/LinearHash.h>
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>

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
    size_t n = 4;
    size_t iterations = 100'000'000;  // clang-format off
  std::vector<std::thread> threads;
  threads.reserve(n);
  ciri::metrics::CounterMinMax<size_t> c(ciri::metrics::CounterMinMax<size_t>::Mode::Max, 4);
  for (size_t i = 0; i < n; ++i) {
    threads.emplace_back([&c, &iterations, num = i]() {
      for (size_t i = 0; i <= iterations; ++i) {
        c.update(num * i);
      }
    });
  }
  for (size_t i = 0; i < n; ++i) {
    threads[i].join();
  }
  EXPECT_NE(c.get(), std::nullopt);
  EXPECT_EQ(c.get().value(), (n - 1) * iterations);
}

TEST(MetricsTests, CounterStress) {
  size_t n = 4;
  size_t iterations = 100'000'000; // clang-format off
  size_t dummy_sum = 0, ciri_sum = 0;
  { // dummy
    std::vector<std::thread> threads;
    threads.reserve(n);
    std::atomic_size_t dummy;
    auto start = std::chrono::system_clock::now();
    for (size_t i = 0; i < n; ++i) {
      threads.emplace_back([&]() {
        for (size_t k = 0; k < iterations; ++k) {
          dummy.fetch_add(k, std::memory_order_relaxed);
        }
      });
    }
    dummy_sum = dummy.load(std::memory_order_relaxed);
    for (size_t i = 0; i < n; ++i) {
      threads[i].join();
    }
    threads.clear();
    dummy_sum = dummy.load(std::memory_order_acquire);
    auto finish = std::chrono::system_clock::now();
    auto duration = finish - start;
    std::cout << "dummy: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms\n";
  }

  { // ciri counter
    std::vector<std::thread> threads;
    threads.reserve(n);
    ciri::metrics::CounterSum c(n);
    auto start = std::chrono::system_clock::now();
    for (size_t i = 0; i < n; ++i) {
      threads.emplace_back([&]() {
        for (size_t k = 0; k < iterations; ++k) {
          c.increase(k);
        }
      });
    }
    for (size_t i = 0; i < n; ++i) {
      threads[i].join();
    }
    threads.clear();
    ciri_sum = c.get();
    auto finish = std::chrono::system_clock::now();
    auto duration = finish - start;
    std::cout << "ciri: " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms\n";

  }
}
