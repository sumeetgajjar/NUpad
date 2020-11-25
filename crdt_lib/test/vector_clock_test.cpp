//
// Created by sumeet on 11/20/20.
//
#include "clock.h"
#include <gtest/gtest.h>
#include <glog/logging.h>


class VectorClockTest : public ::testing::Test {
protected:
    void SetUp() override {
        myPeerId_ = "1";
        clock_ = new nupad::clock::VectorClock(myPeerId_);
    }

    void TearDown() override {
      delete clock_;
    }

  nupad::PeerId myPeerId_;
  nupad::clock::VectorClock *clock_;
};

TEST_F(VectorClockTest, VectorClockOperations) {
    nupad::clock::ClockState expectedClockState = {{myPeerId_, 0}};
    ASSERT_EQ(clock_->getState(), expectedClockState);

    clock_->tick();
    expectedClockState = {{myPeerId_, 1}};
    ASSERT_EQ(clock_->getState(), expectedClockState);

    clock_->tick();
    expectedClockState = {{myPeerId_, 2}};
    ASSERT_EQ(clock_->getState(), expectedClockState);
}

TEST_F(VectorClockTest, UpdateFailsWithLowerTickValue) {
    clock_->update(12);
    ASSERT_DEATH({
                     clock_->update(10);
                 }, "new tick value should be strictly greater than current tick value");
}
