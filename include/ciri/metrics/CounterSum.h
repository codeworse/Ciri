#pragma once
#include <array>
#include <atomic>
#include <cassert>
#include <concepts>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#include "ciri/metrics/CounterBasic.h"

namespace ciri {
namespace metrics {
template <std::integral T = size_t>
class CounterSum {
public:
    CounterSum(size_t size = 1) : counter_(size, 0) {}
    void increase(T inc) { counter_.increase(inc); }
    void decrease(T dec) { counter_.decrease(dec); }
    T getApprox() const {
        T sum = T{};
        auto f = [&](const std::atomic<T>& element) {
            sum += element.load(std::memory_order_relaxed);
        };
        counter_.iterate(f);
        return sum;
    }

    T get() const {
        counter_.sync();
        return getApprox();
    }

    ~CounterSum() = default;

private:
    const bool fast_module = false;
    ciri::metrics::CounterBasic<T> counter_;
};

}  // namespace metrics
}  // namespace ciri
