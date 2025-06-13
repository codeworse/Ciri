#pragma once
#include <atomic>
#include <concepts>
#include <optional>
#include <type_traits>

#include "CounterBasic.h"
#include "ciri/metrics/CounterBasic.h"
namespace ciri {
namespace metrics {

template <std::totally_ordered T>
class CounterMinMax {
public:
    using ValueType = T;
    using StoreType = std::optional<T>;
    using CounterBasicType = CounterBasic<StoreType>;
    enum Mode { Min, Max };
    struct CompareCallback {
        Mode mode;
        ValueType upd;
        void operator()(std::atomic<StoreType>& element) {
            while (true) {
                StoreType value = element.load(std::memory_order_relaxed);

                if (value.has_value() &&
                    ((*value <= upd && mode == Mode::Min) ||
                     (*value >= upd && mode == Mode::Max))) {
                    break;
                }
                if (element.compare_exchange_strong(
                        value, {upd}, std::memory_order_relaxed)) {
                    break;
                }
            }
        }
    };

    struct GetCallback {
        Mode mode;
        std::optional<ValueType> ans{
            std::nullopt};  // Could be nullopt if there are no values
        void operator()(const std::atomic<StoreType>& element) {
            auto value = element.load(std::memory_order_relaxed);
            if (!value.has_value()) {
                return;
            }
            if (!ans.has_value()) {
                ans = *value;
                return;
            }

            if ((mode == Mode::Min && *value < *ans) ||
                (mode == Mode::Max && *value > *ans)) {
                ans = *value;
            }
        }
    };
    CounterMinMax(Mode mode, size_t capacity = 1)
        : mode_(mode), counter_(capacity, std::nullopt) {}

    void update(ValueType upd) {
        CompareCallback callback{.mode = mode_, .upd = upd};
        counter_.call(callback);
    }

    std::optional<ValueType> getApprox() const {
        GetCallback callback{.mode = mode_};
        counter_.iterate(callback);
        return callback.ans;
    }

    std::optional<ValueType> get() const {
        counter_.sync();
        return getApprox();
    }

private:
    Mode mode_;
    CounterBasic<StoreType> counter_;
};
}  // namespace metrics
}  // namespace ciri
