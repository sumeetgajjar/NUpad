//
// Created by sumeet on 11/24/20.
//

#ifndef NUPAD_TESTUTILS_H
#define NUPAD_TESTUTILS_H

#include <random>
#include <chrono>
#include <glog/logging.h>
#include <gtest/gtest.h>

class TestUtils {
    static long randomSeed;
    static std::default_random_engine randomEngine;
public:
    static void init(int argc, char **argv);

    static int getRandomInt(int min = 0, int max = INT32_MAX);
};

long getRandomSeed() {
    using namespace std::chrono;
    milliseconds currentMillis = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    long seed = currentMillis.count();
    return seed;
}

long TestUtils::randomSeed(getRandomSeed());
std::default_random_engine TestUtils::randomEngine(randomSeed);

void TestUtils::init(int argc, char **argv) {
    google::InitGoogleLogging(argv[0]);
    ::testing::InitGoogleTest(&argc, argv);
    LOG(ERROR) << "Seed: " << randomSeed;
}

int TestUtils::getRandomInt(const int min, const int max) {
    std::uniform_int_distribution<int> distribution(min, max);
    return distribution(randomEngine);
}

#endif //NUPAD_TESTUTILS_H
