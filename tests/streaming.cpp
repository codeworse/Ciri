#include <ciri/streaming/CountMinSketch.h>
#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

const std::vector<std::string> test_words = {"The",   "question", "is",  "not",
                                             "where", "but",      "when"};
std::vector<size_t> answer = {0, 0, 0, 0};  // fill in tests
auto run_cms(std::istream& in, double epsilon, double delta) {
    ciri::streaming::CountMinSketch<std::string> cms(epsilon, delta);
    auto start = std::chrono::steady_clock::now();
    size_t N = 0;
    {
        std::string word;
        while (in >> word) {
            N++;
            cms.update(word);
        }
    }
    auto finish = std::chrono::steady_clock::now();
    for (size_t i = 0; i < test_words.size(); ++i) {
        const auto& word = test_words[i];
        size_t freq = cms.get(word);
        size_t diff = freq > answer[i] ? freq - answer[i] : answer[i] - freq;
        double err = (double)(diff) / N;
        std::cout << "\"" << word << "\":" << err << "\n";
    }
    return finish - start;
}

void fill_answer(std::istream& in) {
    std::unordered_map<std::string, size_t> cnt;
    std::string word;
    while (in >> word) {
        cnt[word]++;
    }
    for (size_t i = 0; i < test_words.size(); ++i) {
        const auto& word = test_words[i];
        answer[i] = cnt[word];
    }
}

TEST(StreamingTests, CountMinSketch) {
    std::string iliad_file = std::getenv("ILIAD_FILE");
    std::filesystem::path p{iliad_file.c_str()};
    EXPECT_TRUE(std::filesystem::exists(p));
    std::cout << "using iliad file: " << iliad_file << "\n";
    {  /// fill answer
        std::ifstream iliad(iliad_file);
        fill_answer(iliad);
    }

    std::cout << "finish filling\n";

    {  /// cms
        std::ifstream iliad(iliad_file);
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                           run_cms(iliad, 0.1, 0.1))
                           .count();
        std::cout << "CMS time:" << elapsed << "ms\n";
    }
}
