//
// Created by sumeet on 11/23/20.
//

#include "operation.h"
#include "test_utils.h"
#include <gtest/gtest.h>

nupad::common::ID getID(const std::string &peerId, const nupad::clock::Tick tick) {
    nupad::common::ID id;
    id.set_peer_id(peerId);
    id.mutable_tick()->set_value(tick);
    return id;
}

TEST(IDTestSuite, DISABLED_NegativeTickFails) {
    ASSERT_DEATH({
                     getID("1", -1);
                 }, "tick cannot be negative: -1");
}

TEST(IDTestSuite, ZeroTickWorks) {
    nupad::common::ID elementId = getID("1", 0);
    ASSERT_EQ(elementId.tick().value(), 0);
}

TEST(IDTestSuite, GettersTest) {
    nupad::PeerId randomPeerId(std::to_string(TestUtils::getRandomInt()));
    nupad::clock::Tick randomTick = TestUtils::getRandomInt();
    nupad::common::ID elementId = getID(randomPeerId, randomTick);
    ASSERT_EQ(elementId.peer_id(), randomPeerId);
    ASSERT_EQ(elementId.tick().value(), randomTick);
}

TEST(IDTestSuite, EqualityTest) {
    using namespace nupad::common;
    nupad::clock::Tick randomTick = TestUtils::getRandomInt();
    ID elementId1 = getID("1", randomTick);
    ID elementId2 = getID("1", randomTick);
    ID elementId3 = getID("1", randomTick);

    // Reflexivity
    ASSERT_EQ(elementId1, elementId1);

    // Associativity
    ASSERT_EQ(elementId1, elementId2);
    ASSERT_EQ(elementId2, elementId1);

    // Transitivity
    ASSERT_EQ(elementId1, elementId2);
    ASSERT_EQ(elementId2, elementId3);
}

TEST(IDTestSuite, IneqaulityTest) {
    using namespace nupad::common;
    nupad::clock::Tick randomTick = TestUtils::getRandomInt();
    ID elementId1 = getID("1", randomTick);
    ID elementId2 = getID("2", randomTick);
    // PeerIds are not equal
    ASSERT_NE(elementId1, elementId2);

    // Ticks are not equal
    ID elementId3 = getID("2", 1);
    ASSERT_NE(elementId2, elementId3);
}

TEST(IDTestSuite, LessThanTest) {
    using namespace nupad::common;
    nupad::clock::Tick randomTick = TestUtils::getRandomInt();
    // PeerId Less Than
    ID elementId1 = getID("1", randomTick);
    ID elementId2 = getID("2", randomTick);
    ID elementId3 = getID("2", randomTick);
    ASSERT_LT(elementId1, elementId2);

    ASSERT_LE(elementId1, elementId1);
    ASSERT_LE(elementId1, elementId2);
    ASSERT_LE(elementId2, elementId3);

    // Tick Less Than
    ID elementId4 = getID("1", 1);
    ID elementId5 = getID("1", 2);
    ID elementId6 = getID("1", 2);
    ASSERT_LT(elementId4, elementId5);

    ASSERT_LE(elementId4, elementId4);
    ASSERT_LE(elementId4, elementId5);
    ASSERT_LE(elementId5, elementId6);
}

TEST(IDTestSuite, GreaterThanTest) {
    using namespace nupad::common;
    nupad::clock::Tick randomTick = TestUtils::getRandomInt();
    // PeerId Greater Than
    ID elementId1 = getID("1", randomTick);
    ID elementId2 = getID("2", randomTick);
    ID elementId3 = getID("2", randomTick);
    ASSERT_GT(elementId2, elementId1);

    ASSERT_GE(elementId1, elementId1);
    ASSERT_GE(elementId2, elementId1);
    ASSERT_GE(elementId3, elementId2);

    // Tick Greater Than
    ID elementId4 = getID("1", 1);
    ID elementId5 = getID("1", 2);
    ID elementId6 = getID("1", 2);
    ASSERT_GT(elementId5, elementId4);

    ASSERT_GE(elementId4, elementId4);
    ASSERT_GE(elementId5, elementId4);
    ASSERT_GE(elementId6, elementId5);
}