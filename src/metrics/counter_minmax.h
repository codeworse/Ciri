#pragma once
#include <concepts>
#include <type_traits>
#include "metrics/counter_basic.h"
namespace ciri {
namespace metrics {
template<typename T>
concept ComparedAndMovable = std::totally_ordered<T> && std::is_move_assignable_v<T>;

template<ComparedAndMovable T>
class CounterMinMax {
public:
    enum Mode {
        Min,
        Max
    };
    CounterMinMax(Mode mode, size_t capacity = 1) : mode_(mode), counter_(capacity) {}

    void update(T upd) {
        auto min_func = [&](const std::atomic<T>& element) -> bool {
            T value = element.load(std::memory_order_relaxed);
            return value > upd;
        };
        auto max_func = [&](const std::atomic<T>& element) -> bool {
            T value = element.load(std::memory_order_relaxed);
            return value < upd;
        };
        switch (mode_) {
        case Mode::Max:
            counter_.set_if(upd, max_func);
            return;
        case Mode::Min:
            counter_.set_if(upd, min_func);
            return;
        }
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
}
}