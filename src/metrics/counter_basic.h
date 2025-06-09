#pragma once
#include <array>
#include <atomic>
#include <cassert>
#include <concepts>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <vector>

namespace ciri {
namespace metrics {

template <std::default_initializable T>
class CounterBasic {
 public:
  static const size_t alignment = 64;
  CounterBasic(size_t capacity)
      : fast_module((capacity & (capacity - 1)) == 0), capacity_(capacity) {
    assert(capacity_ > 0);
    counters_ = static_cast<std::atomic<T>*>(std::aligned_alloc(
        alignment, capacity * sizeof(decltype(counters_[0]))));
    assert(counters_ != nullptr);
    for (size_t i = 0; i < capacity_; ++i) {
      new (counters_ + i) std::atomic<T>();
    }
    for (size_t i = 0; i < capacity_; ++i) {
      counters_[i].store(0);
    }
  }
  void increase(T inc) {
    size_t thread_hash =
        std::hash<std::thread::id>{}(std::this_thread::get_id());
    counters_[get_index(thread_hash)].fetch_add(inc, std::memory_order_relaxed);
  }
  void decrease(T dec) {
    size_t thread_hash =
        std::hash<std::thread::id>{}(std::this_thread::get_id());
    counters_[get_index(thread_hash)].fetch_sub(dec, std::memory_order_relaxed);
  }
  void set(T value) {
    size_t thread_hash =
        std::hash<std::thread::id>{}(std::this_thread::get_id());
    counters_[get_index(thread_hash)].store(value, std::memory_order_relaxed);
  }

  template <std::invocable<std::atomic<T>&> F>
  void call(F&& callback) {
    size_t thread_hash =
        std::hash<std::thread::id>{}(std::this_thread::get_id());
    callback(counters_[get_index(thread_hash)]);
  }

  inline void get_index() const {
    size_t thread_hash =
        std::hash<std::thread::id>{}(std::this_thread::get_id());
    return get_index(thread_hash);
  }

  template <std::invocable<std::atomic<T>&> F>
  void iterate(F&& func) {
    for (size_t i = 0; i < capacity_; ++i) {
      func(counters_[i]);
    }
  }

  template <std::invocable<const std::atomic<T>&> F>
  void iterate(F&& func) const {
    for (size_t i = 0; i < capacity_; ++i) {
      func(counters_[i]);
    }
  }

  ~CounterBasic() { std::free(counters_); }

  inline size_t get_index(size_t i) const {
    return fast_module ? (i & (capacity_ - 1)) : (i % capacity_);
  }
  const bool fast_module = false;
  const size_t capacity_{0};
  alignas(alignment) std::atomic<T>* counters_{nullptr};
};
}  // namespace metrics
}  // namespace ciri
