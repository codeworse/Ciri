#include <ciri/utils/LinearHash.h>
#include <gtest/gtest.h>

TEST(UtilsTests, LinearHash) {
    ciri::utils::LinearHash h;
    h.hash(1);
}
