//
// Created by sumeet on 11/20/20.
//
#include "clock.h"
#include <gtest/gtest.h>
#include <glog/logging.h>


class VectorClockTest : public ::testing::Test {
protected:
    static const nupad::PeerId myPeerId_;

    static void SetUpTestSuite() {
        nupad::clock::VectorClock::init(myPeerId_);
    }
};

const nupad::PeerId VectorClockTest::myPeerId_("1");

TEST_F(VectorClockTest, DoubleInitializationFailure) {
    ASSERT_DEATH({
                     nupad::clock::VectorClock::init(myPeerId_);
                 }, "VectorClock already initialized");
}

TEST_F(VectorClockTest, VectorClockOperations) {
    nupad::clock::ClockState expectedClockState = {{myPeerId_, 0}};
    ASSERT_EQ(nupad::clock::VectorClock::getState(), expectedClockState);

    nupad::clock::VectorClock::tick();
    expectedClockState = {{myPeerId_, 1}};
    ASSERT_EQ(nupad::clock::VectorClock::getState(), expectedClockState);

    nupad::clock::VectorClock::tick();
    expectedClockState = {{myPeerId_, 2}};
    ASSERT_EQ(nupad::clock::VectorClock::getState(), expectedClockState);

    const nupad::PeerId otherPeerId("random");
    const nupad::clock::Tick otherPeerTick = 198;
    nupad::clock::VectorClock::update(otherPeerId, otherPeerTick);
    expectedClockState = {{myPeerId_,   2},
                          {otherPeerId, otherPeerTick}};
    ASSERT_EQ(nupad::clock::VectorClock::getTick(otherPeerId), otherPeerTick);
    ASSERT_EQ(nupad::clock::VectorClock::getState(), expectedClockState);

    nupad::clock::VectorClock::update(otherPeerId, otherPeerTick + 1);
    ASSERT_EQ(nupad::clock::VectorClock::getTick(otherPeerId), otherPeerTick + 1);
}

TEST_F(VectorClockTest, UpdateFailsWithLowerTickValue) {
    ASSERT_DEATH({
                     const nupad::PeerId otherPeerId("random");
                     const nupad::clock::Tick otherPeerTick = 19;
                     nupad::clock::VectorClock::update(otherPeerId, otherPeerTick);
                     nupad::clock::VectorClock::update(otherPeerId, otherPeerTick - 1);
                 }, "new tick value should be strictly greater than current tick value");
}

int main(int argc, char **argv) {
    google::InitGoogleLogging(argv[0]);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
