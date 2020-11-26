//
// Created by sumeet on 11/24/20.
//

#ifndef NUPAD_TESTUTILS_H
#define NUPAD_TESTUTILS_H

#include <random>
#include <chrono>

class TestUtils {
    static long randomSeed;
    static std::default_random_engine randomEngine;
public:
    static void init(int argc, char **argv);

    static int getRandomInt(int min = 0, int max = INT32_MAX);
};

#endif //NUPAD_TESTUTILS_H
