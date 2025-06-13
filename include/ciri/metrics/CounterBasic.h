#pragma once
#include <array>
#include <atomic>
#include <cassert>
#include <concepts>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

#include "ciri/utils/AlignedStorage.h"

namespace ciri {
namespace metrics {

template <std::default_initializable T>
class CounterBasic {
public:
    static constexpr size_t alignment = 64;
    using ValueType = T;
    template <typename... Args>
    CounterBasic(size_t size, Args... args)
        : fast_module((size & (size - 1)) == 0),
          size_(size),
          counters_(size, std::forward<decltype(args)>(args)...) {
        assert(size_ > 0);
    }
    void increase(T inc) {
        size_t thread_hash =
            std::hash<std::thread::id>{}(std::this_thread::get_id());
        counters_[getIndex(thread_hash)].fetch_add(inc,
                                                   std::memory_order_relaxed);
    }
    void decrease(T dec) {
        size_t thread_hash =
            std::hash<std::thread::id>{}(std::this_thread::get_id());
        counters_[getIndex(thread_hash)].fetch_sub(dec,
                                                   std::memory_order_relaxed);
    }
    void set(T value) {
        size_t thread_hash =
            std::hash<std::thread::id>{}(std::this_thread::get_id());
        counters_[getIndex(thread_hash)].store(value,
                                               std::memory_order_relaxed);
    }

    template <std::invocable<std::atomic<T>&> F>
    void call(F&& callback) {
        size_t thread_hash =
            std::hash<std::thread::id>{}(std::this_thread::get_id());
        callback(counters_[getIndex(thread_hash)]);
    }

    inline void getIndex() const {
        size_t thread_hash =
            std::hash<std::thread::id>{}(std::this_thread::get_id());
        return getIndex(thread_hash);
    }

    template <std::invocable<std::atomic<T>&> F>
    void iterate(F&& func) {
        for (size_t i = 0; i < size_; ++i) {
            func(counters_[i]);
        }
    }

    template <std::invocable<const std::atomic<T>&> F>
    void iterate(F&& func) const {
        for (size_t i = 0; i < size_; ++i) {
            func(counters_[i]);
        }
    }

    void sync() const { std::atomic_thread_fence(std::memory_order_acquire); }

    ~CounterBasic() = default;

private:
    inline size_t getIndex(size_t i) const {
        return fast_module ? (i & (size_ - 1)) : (i % size_);
    }
    const bool fast_module = false;
    const size_t size_{0};
    ciri::utils::AlignedStorage<std::atomic<T>, alignment> counters_;
};
}  // namespace metrics
}  // namespace ciri
