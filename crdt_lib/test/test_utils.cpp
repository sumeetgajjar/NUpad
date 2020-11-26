//
// Created by madhurjain on 11/26/20.
//
#include "test_utils.h"
#include <glog/logging.h>
#include <gtest/gtest.h>

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