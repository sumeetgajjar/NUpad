//
// Created by madhurjain on 11/25/20.
//

#include "list.h"
#include "gtest/gtest.h"

class ListTest : public ::testing::Test {
protected:
  void SetUp() override {
    nupad::PeerId peerId1 = "1";
    nupad::Context ctx1(peerId1, nupad::clock::VectorClock(peerId1));
    list1_ = new nupad::crdt::DoublyLinkedList<char>(ctx1);
    nupad::PeerId peerId2 = "2";
    nupad::Context ctx2(peerId2, nupad::clock::VectorClock(peerId2));
    list2_ = new nupad::crdt::DoublyLinkedList<char>(ctx2);
  }

  void TearDown() override {
    delete list1_;
    delete list2_;
  }

  nupad::crdt::DoublyLinkedList<char> *list1_;
  nupad::crdt::DoublyLinkedList<char> *list2_;
};

TEST_F(ListTest, ListInitTest) {
  EXPECT_TRUE(list1_ != nullptr);
  ASSERT_EQ(list1_->size(), 0);
  std::vector<char> expectedElements = {};
  ASSERT_EQ(list1_->getContents(), expectedElements);
}

TEST_F(ListTest, ListInsertTest) {
  auto insertOp = list1_->insert(0, 'a');
  ASSERT_EQ(list1_->size(), 1);
  std::vector<char> expectedElements{'a'};
  ASSERT_EQ(list1_->getContents(), expectedElements);
}

TEST_F(ListTest, ListInsertRemoveTest) {
  nupad::crdt::Operation insertOp = list1_->insert(0, 'a');
  nupad::crdt::Operation deleteOp = list1_->remove(0);
  ASSERT_EQ(list1_->size(), 0);
  std::vector<char> expectedElements{};
  ASSERT_EQ(list1_->getContents(), expectedElements);
}

TEST_F(ListTest, ListFailInsert) {
  ASSERT_DEATH(list1_->insert(1, 'a'), "index cannot be greater");
}

TEST_F(ListTest, ListFailRemove) {
  ASSERT_DEATH(list1_->remove(1), "index cannot be greater");
}

TEST_F(ListTest, ListRemoteInsertTest) {
  // get the operation from a local insert
  for(int i = 0; i < 4; i++) {
    list1_->insert(i, 'a' + i);
    list2_->insert(i, 'a' + i);
  }
  ASSERT_EQ(list1_->size(), 4);
  ASSERT_EQ(list2_->size(), 4);
  std::vector<char> expectedElements{'a', 'b', 'c', 'd'};
  ASSERT_EQ(list2_->getContents(), expectedElements);
  ASSERT_EQ(list1_->getContents(), expectedElements);
}

TEST_F(ListTest, ListInsertSameIndexTest) {
  list1_->insert(0, 'a');
  list1_->insert(1, 'b');
  list1_->insert(1, 'c');
  ASSERT_EQ(list1_->size(), 3);
  std::vector<char> expectedElements{'a', 'c', 'b'};
  ASSERT_EQ(list1_->getContents(), expectedElements);
}