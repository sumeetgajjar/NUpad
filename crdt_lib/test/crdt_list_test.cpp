//
// Created by sumeet on 11/23/20.
//

#include "operation.h"
#include "test_utils.h"
#include <gtest/gtest.h>

TEST(ElementIdTestSuite, NegativeTickFails) {
    ASSERT_DEATH({
                     nupad::crdt::ElementId("1", -1);
                 }, "tick cannot be negative: -1");
}

TEST(ElementIdTestSuite, ZeroTickWorks) {
    nupad::crdt::ElementId elementId("1", 0);
    ASSERT_EQ(elementId.getTick(), 0);
}

TEST(ElementIdTestSuite, GettersTest) {
    nupad::PeerId randomPeerId(std::to_string(TestUtils::getRandomInt()));
    nupad::clock::Tick randomTick = TestUtils::getRandomInt();
    nupad::crdt::ElementId elementId(randomPeerId, randomTick);
    ASSERT_EQ(elementId.getPeerId(), randomPeerId);
    ASSERT_EQ(elementId.getTick(), randomTick);
}

TEST(ElementIdTestSuite, EqualityTest) {
    using namespace nupad::crdt;
    nupad::clock::Tick randomTick = TestUtils::getRandomInt();
    ElementId elementId1("1", randomTick);
    ElementId elementId2("1", randomTick);
    ElementId elementId3("1", randomTick);

    // Reflexivity
    ASSERT_EQ(elementId1, elementId1);

    // Associativity
    ASSERT_EQ(elementId1, elementId2);
    ASSERT_EQ(elementId2, elementId1);

    // Transitivity
    ASSERT_EQ(elementId1, elementId2);
    ASSERT_EQ(elementId2, elementId3);
}

TEST(ElementIdTestSuite, IneqaulityTest) {
    using namespace nupad::crdt;
    nupad::clock::Tick randomTick = TestUtils::getRandomInt();
    ElementId elementId1("1", randomTick);
    ElementId elementId2("2", randomTick);
    // PeerIds are not equal
    ASSERT_NE(elementId1, elementId2);

    // Ticks are not equal
    ElementId elementId3("2", 1);
    ASSERT_NE(elementId2, elementId3);
}

TEST(ElementIdTestSuite, LessThanTest) {
    using namespace nupad::crdt;
    nupad::clock::Tick randomTick = TestUtils::getRandomInt();
    // PeerId Less Than
    ElementId elementId1("1", randomTick);
    ElementId elementId2("2", randomTick);
    ElementId elementId3("2", randomTick);
    ASSERT_LT(elementId1, elementId2);

    ASSERT_LE(elementId1, elementId1);
    ASSERT_LE(elementId1, elementId2);
    ASSERT_LE(elementId2, elementId3);

    // Tick Less Than
    ElementId elementId4("1", 1);
    ElementId elementId5("1", 2);
    ElementId elementId6("1", 2);
    ASSERT_LT(elementId4, elementId5);

    ASSERT_LE(elementId4, elementId4);
    ASSERT_LE(elementId4, elementId5);
    ASSERT_LE(elementId5, elementId6);
}

TEST(ElementIdTestSuite, GreaterThanTest) {
    using namespace nupad::crdt;
    nupad::clock::Tick randomTick = TestUtils::getRandomInt();
    // PeerId Greater Than
    ElementId elementId1("1", randomTick);
    ElementId elementId2("2", randomTick);
    ElementId elementId3("2", randomTick);
    ASSERT_GT(elementId2, elementId1);

    ASSERT_GE(elementId1, elementId1);
    ASSERT_GE(elementId2, elementId1);
    ASSERT_GE(elementId3, elementId2);

    // Tick Greater Than
    ElementId elementId4("1", 1);
    ElementId elementId5("1", 2);
    ElementId elementId6("1", 2);
    ASSERT_GT(elementId5, elementId4);

    ASSERT_GE(elementId4, elementId4);
    ASSERT_GE(elementId5, elementId4);
    ASSERT_GE(elementId6, elementId5);
}

int main(int argc, char **argv) {
    TestUtils::init(argc, argv);
    RUN_ALL_TESTS();
}