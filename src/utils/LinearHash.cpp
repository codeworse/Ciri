#include "ciri/utils/LinearHash.h"
#include <ctime>
#include <random>

namespace ciri {
namespace utils {

LinearHash::LinearHash() {
    std::mt19937_64 gn;
    gn.seed(time(0));
    while (a_ == 0) {
        a_ = gn();
    }
    b_ = gn();
}

uint64_t LinearHash::hash(uint32_t value) const {
    return ((uint64_t)a_ * (uint64_t)value + (uint64_t)b_) % p;
}
}  // namespace utils
}  // namespace ciri
