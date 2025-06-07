#pragma once
#include <atomic>
#include <concepts>
#include <vector>
#include <array>
#include <cstdlib>
#include <thread>
#include <cassert>
#include <cstring>

namespace ciri {
namespace metrics {
template<std::integral T>
class Counter {
public:
    static const size_t alignment = 64;
    Counter(size_t capacity) : 
        fast_module((capacity & (capacity - 1)) == 0)
        , capacity_(capacity) {
        assert(capacity_ > 0);
        counters_ = static_cast<std::atomic<T>*>(std::aligned_alloc(alignment, capacity * sizeof(decltype(counters_[0]))));
        for (size_t i = 0; i < capacity_; ++i) {
            counters_[i].store(0);
        }
    }
    void increase(T inc) {
        size_t thread_hash = std::hash<std::thread::id>{}(std::this_thread::get_id());
        counters_[thread_hash % capacity_].fetch_add(inc, std::memory_order_relaxed);
    }
    void decrease(T dec) {
        size_t thread_hash = std::hash<std::thread::id>{}(std::this_thread::get_id());
        counters_[thread_hash % capacity_].fetch_sub(dec, std::memory_order_relaxed);
    }
    T get() const {
        T sum{0};
        for (size_t i = 0; i < capacity_; ++i) {
            sum += counters_[i].load(std::memory_order_relaxed);
        }
        return sum;
    }

    ~Counter() {
        std::free(counters_);
    }
private:
    inline size_t get_index(size_t i) const {
        return fast_module ? (i & (capacity_ - 1)) : (i % capacity_);
    }
    const bool fast_module = false;
    const size_t capacity_;
    alignas(alignment) std::atomic<T>* counters_;
};

}
}