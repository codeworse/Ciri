#pragma once
#include <concepts>
#include <cstdint>
namespace ciri {
namespace utils {
// LinearHash is a easy way to hash some intergal value:
// hash(x) = ((a * x + b) % p), where p is some prime number.
// Also, p should be more than any possible x.
// By now uint32_t is the largest supported type
class LinearHash {
public:
    LinearHash();
    uint64_t hash(uint32_t value) const;

private:
    static constexpr uint64_t p = (1ull << 61) - 1;
    uint32_t a_{0}, b_{0};
};
}  // namespace utils
}  // namespace ciri
