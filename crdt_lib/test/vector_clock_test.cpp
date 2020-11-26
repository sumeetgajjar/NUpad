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

TEST(VectorInitTest, EmptyPeerIdTest) {
  ASSERT_DEATH(nupad::clock::VectorClock(""), "myPeerId cannot be empty");
}

TEST_F(VectorClockTest, VectorClockOperations) {
    nupad::clock::ClockState expectedClockState = {{myPeerId_, 0}};
    ASSERT_EQ(clock_->getState(), expectedClockState);

    clock_->tick();
    expectedClockState = {{myPeerId_, 1}};
    ASSERT_EQ(clock_->getState(), expectedClockState);

    clock_->tick();
    expectedClockState = {{myPeerId_, 2}};
    ASSERT_EQ(clock_->getState(), expectedClockState);

    const nupad::PeerId otherPeerId("random");
    const nupad::clock::Tick otherPeerTick = 198;
    clock_->update(otherPeerId, otherPeerTick);
    expectedClockState = {{myPeerId_,   2},
                          {otherPeerId, otherPeerTick}};
    ASSERT_EQ(clock_->getTick(otherPeerId), otherPeerTick);
    ASSERT_EQ(clock_->getState(), expectedClockState);

    clock_->update(otherPeerId, otherPeerTick + 1);
    ASSERT_EQ(clock_->getTick(otherPeerId), otherPeerTick + 1);
}

TEST_F(VectorClockTest, UpdateOwnTickFailTest) {
    ASSERT_DEATH(clock_->update(myPeerId_, 12), "Cannot update my own tick");
}

TEST_F(VectorClockTest, UpdateFailsWithLowerTickValue) {
    nupad::PeerId otherPeerId{"2"};
    clock_->update(otherPeerId, 12);
    ASSERT_DEATH(clock_->update(otherPeerId, 10), "new tick value should be "
                                                  "strictly greater than current tick value");
}