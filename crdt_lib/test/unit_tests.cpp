//
// Created by sumeet on 11/20/20.
//
#include <csignal>
#include "clock.h"
#include <gtest/gtest.h>

TEST(VectorClockTest, DoubleInitializationFailure) {
    ASSERT_DEATH({
                     nupad::clock::VectorClock::init();
                     nupad::clock::VectorClock::init();
                 }, "VectorClock already initialized");
}