#include "metrics/counter.h"
#include <gtest/gtest.h>

TEST(MetricsTests, CounterBasics) {
    ciri::metrics::Counter<size_t> c;
    for (size_t i = 0; i < 100; ++i) {
        c.increase(10);
    }

    for (size_t i = 0; i < 100; ++i) {
        c.decrease(10);
    }

    EXPECT_EQ(c.get(), 0);
}

