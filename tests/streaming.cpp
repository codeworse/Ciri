#include <ciri/streaming/CountMinSketch.h>
#include <gtest/gtest.h>
TEST(StreamingTests, CountMinSketch) {
    ciri::streaming::CountMinSketch<int> cms(.5, .5);
    cms.update(1);
    int ans = cms.get(1);
}
