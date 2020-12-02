//
// Created by madhurjain on 11/25/20.
//

#include "list.h"
#include "gtest/gtest.h"

class ListTestSuite : public ::testing::Test {
protected:
    void SetUp() override {
        list1_ = new nupad::crdt::DoublyLinkedList([&]() { return getNextElementID("1"); });
        list2_ = new nupad::crdt::DoublyLinkedList([&]() { return getNextElementID("2"); });
        op_ = new nupad::common::Operation;
    }

    void TearDown() override {
        delete list1_;
        delete list2_;
        delete op_;
    }

    nupad::common::ID getNextElementID(const nupad::PeerId &peerId) {
        auto tick = nextElementId_[peerId]++;
        nupad::common::ID id;
        id.set_peer_id(peerId);
        id.mutable_tick()->set_value(tick);
        return id;
    }

    nupad::crdt::DoublyLinkedList *list1_;
    nupad::crdt::DoublyLinkedList *list2_;
    std::unordered_map<nupad::PeerId, nupad::clock::Tick> nextElementId_;
    nupad::common::Operation *op_;
};

TEST_F(ListTestSuite, ListInitTest) {
    EXPECT_TRUE(list1_ != nullptr);
    ASSERT_EQ(list1_->size(), 0);
    std::string expectedElements = {};
    ASSERT_EQ(list1_->getContents(), expectedElements);
}

TEST_F(ListTestSuite, ListInsertTest) {
    list1_->insert(0, 'a', op_);
    ASSERT_EQ(list1_->size(), 1);
    std::string expectedElements{'a'};
    ASSERT_EQ(list1_->getContents(), expectedElements);
}

TEST_F(ListTestSuite, ListInsertRemoveTest) {
    list1_->insert(0, 'a', op_);
    list1_->remove(0, op_);
    ASSERT_EQ(list1_->size(), 0);
    std::string expectedElements{};
    ASSERT_EQ(list1_->getContents(), expectedElements);
}

TEST_F(ListTestSuite, ListFailInsert) {
    ASSERT_DEATH(list1_->insert(1, 'a', op_), "index cannot be greater");
}

TEST_F(ListTestSuite, ListFailRemove) {
    ASSERT_DEATH(list1_->remove(1, op_), "index cannot be greater");
}

TEST_F(ListTestSuite, ListRemoteInsertTest) {
    // get the operation from a local insert
    for (int i = 0; i < 4; i++) {
        list1_->insert(i, 'a' + i, op_);
        list2_->insert(i, 'a' + i, op_);
    }
    ASSERT_EQ(list1_->size(), 4);
    ASSERT_EQ(list2_->size(), 4);
    std::string expectedElements{'a', 'b', 'c', 'd'};
    ASSERT_EQ(list2_->getContents(), expectedElements);
    ASSERT_EQ(list1_->getContents(), expectedElements);
}

TEST_F(ListTestSuite, ListInsertSameIndexTest) {
    list1_->insert(0, 'a', op_);
    list1_->insert(1, 'b', op_);
    list1_->insert(1, 'c', op_);
    ASSERT_EQ(list1_->size(), 3);
    std::string expectedElements{'a', 'c', 'b'};
    ASSERT_EQ(list1_->getContents(), expectedElements);
}