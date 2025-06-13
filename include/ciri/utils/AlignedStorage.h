#pragma once
#include <cassert>
#include <concepts>
#include <cstdlib>
namespace ciri {
namespace utils {

template <typename T, size_t alignment = 64>
class AlignedStorage {
public:
    using ValueType = T;
    struct alignas(alignment) AlignedValue {
        ValueType value;
        template <typename... Args>
        AlignedValue(Args&&... args)
            : value(std::forward<decltype(args)>(args)...) {}
    };
    template <typename... Args>
    AlignedStorage(size_t size, Args&&... args) : size_(size) {
        static_assert(sizeof(decltype(data_[0])) == alignment);
        data_ = static_cast<AlignedValue*>(
            std::aligned_alloc(alignment, size * sizeof(decltype(data_[0]))));
        assert(data_ != nullptr);
        for (size_t i = 0; i < size; ++i) {
            new (data_ + i) AlignedValue(std::forward<Args&&...>(args)...);
        }
    }

    ValueType& operator[](size_t i) { return data_[i].value; }

    const ValueType& operator[](size_t i) const { return data_[i].value; }

    ~AlignedStorage() { std::free(data_); }

private:
    size_t size_;
    AlignedValue* data_;
};
}  // namespace utils
}  // namespace ciri
