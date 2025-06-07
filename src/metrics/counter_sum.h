#pragma once
#include <atomic>
#include <concepts>
#include <vector>
#include <array>
#include <cstdlib>
#include <thread>
#include <cassert>
#include <cstring>
#include "metrics/counter_basic.h"

namespace ciri {
namespace metrics {
template<std::integral T = size_t>
class CounterSum {
public:
    static const size_t alignment = 64;
    CounterSum(size_t capacity = 1) : counter_(capacity) {
    }
    void increase(T inc) {
        counter_.increase(inc);
    }
    void decrease(T dec) {
        counter_.decrease(dec);
    }
    T get() const {
        T sum = T{};
        auto f = [&](const T& element) {
            sum += element;
        };
        counter_.get(f);
        return sum;
    }

    ~CounterSum() = default;
private:
    const bool fast_module = false;
    ciri::metrics::CounterBasic<T> counter_;
};

}
}