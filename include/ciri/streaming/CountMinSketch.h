#include <atomic>
#include <cmath>
#include <concepts>
#include <csignal>
#include <type_traits>
#include <vector>
#include "ciri/utils/AlignedStorage.h"
#include "ciri/utils/LinearHash.h"

namespace ciri {
namespace streaming {
// Count Min Sketch algorithm implementation
// for computing the frequency of the element
// with such error:
// P[|real_frequency - cms_frequency| >= epsilon * N] < delta,
// cms_frequency - the frequency, which algorithm compute,
// N - number of added elements(the size of stream),
// epsilon, delta - parameters
template <typename T>
class CountMinSketch {
public:
    using ValueType = T;

    CountMinSketch(double epsilon, double delta)
        : rows_((size_t)std::ceil(std::log((double)1 / delta))),
          columns_((size_t)std::ceil(M_Ef64 / epsilon)),
          hashs_(rows_),
          data_(rows_ * columns_) {}

    void update(ValueType value, size_t upd = 1) {
        size_t hash = std::hash<ValueType>{}(value);
        uint32_t lower = (hash & ((1ull << 32) - 1));
        [[maybe_unused]] uint32_t upper = (hash >> 32);
        for (size_t i = 0; i < rows_; ++i) {
            size_t column = hashs_[i].hash(lower) % columns_;
            data_[i * columns_ + column].fetch_add(upd,
                                                   std::memory_order_relaxed);
        }
    }

    size_t get(ValueType value) const {
        size_t hash = std::hash<ValueType>{}(value);
        uint32_t lower = (hash & ((1ull << 32) - 1));
        [[maybe_unused]] uint32_t upper = (hash >> 32);
        size_t result = 0;
        for (size_t i = 0; i < rows_; ++i) {
            size_t column = hashs_[i].hash(lower) % columns_;
            auto counter =
                data_[i * columns_ + column].load(std::memory_order_relaxed);
            result = result > counter ? counter : result;
        }
        return result;
    }

    ~CountMinSketch() = default;

private:
    size_t rows_, columns_;
    std::vector<utils::LinearHash> hashs_;
    ciri::utils::AlignedStorage<std::atomic<size_t>> data_;
};
}  // namespace streaming
}  // namespace ciri
