#pragma once
#include <concepts>
#include <type_traits>
#include "metrics/counter_basic.h"
namespace ciri {
namespace metrics {

template <std::totally_ordered T>
class CounterMinMax {
 public:
  enum Mode { Min, Max };
  struct CompareCallback {
    Mode mode;
    T upd;
    void operator()(std::atomic<T>& element) {
      while (true) {
        T value = element.load(std::memory_order_relaxed);
        if ((value <= upd && mode == Mode::Min) ||
            (value >= upd && mode == Mode::Max)) {
          break;
        }
        if (element.compare_exchange_strong(value, upd,
                                            std::memory_order_relaxed)) {
          break;
        }
      }
    }
  };
  CounterMinMax(Mode mode, size_t capacity = 1)
      : mode_(mode), counter_(capacity) {}

  void update(T upd) {
    CompareCallback callback{.mode = mode_, .upd = std::move(upd)};
    counter_.call(callback);
  }

  T get() const {
    T result = T{};
    auto max_func = [&](const std::atomic<T>& element) {
      T value = element.load(std::memory_order_relaxed);
      result = result > value ? result : value;
    };
    auto min_func = [&](const std::atomic<T>& element) {
      T value = element.load(std::memory_order_relaxed);
      result = result < value ? result : value;
    };
    switch (mode_) {
      case Mode::Max:
        counter_.iterate(max_func);
        return result;
      case Mode::Min:
        counter_.iterate(min_func);
        return result;
    }
    assert(false);
  }

 private:
  Mode mode_;
  CounterBasic<T> counter_;
};
}  // namespace metrics
}  // namespace ciri