//
// Created by madhurjain on 11/25/20.
//

#include "list.h"
#include "gtest/gtest.h"

class ListTest : public ::testing::Test {
protected:
  void SetUp() override {
    nupad::PeerId myPeerId_ = "1";
    nupad::Context ctx_(myPeerId_, nupad::clock::VectorClock(myPeerId_));
    list_ = new nupad::crdt::DoublyLinkedList<char>(ctx_);
  }

  void TearDown() override { delete list_; }

  nupad::crdt::DoublyLinkedList<char> *list_;
};

TEST_F(ListTest, ListInitTest) {
  EXPECT_TRUE(list_ != nullptr);
  ASSERT_EQ(list_->size(), 0);
  std::vector<char> expectedElements = {};
  ASSERT_EQ(list_->getContents(), expectedElements);
}

TEST_F(ListTest, ListInsertTest) {
  auto insertOp = list_->insert(0, 'a');
  ASSERT_EQ(list_->size(), 1);
  std::vector<char> expectedElements{'a'};
  ASSERT_EQ(list_->getContents(), expectedElements);
}

TEST_F(ListTest, ListInsertRemoveTest) {
  nupad::crdt::Operation insertOp = list_->insert(0, 'a');
  nupad::crdt::Operation deleteOp = list_->remove(0);
  ASSERT_EQ(list_->size(), 0);
  std::vector<char> expectedElements{};
  ASSERT_EQ(list_->getContents(), expectedElements);
}

TEST_F(ListTest, ListFailInsert) {
  ASSERT_DEATH(list_->insert(1, 'a'), "index cannot be greater");
}

TEST_F(ListTest, ListFailRemove) {
  ASSERT_DEATH(list_->remove(1), "index cannot be greater");
}

TEST_F(ListTest, ListRemoteInsertTest) {
  // get the operation from a local insert
  const nupad::crdt::InsertOperation insertOp = list_->insert(0, 'a');
  nupad::PeerId remotePeerId = "2";
  nupad::Context remote_ctx(remotePeerId, nupad::clock::VectorClock
                          (remotePeerId));
  auto remote_list = new nupad::crdt::DoublyLinkedList<char>(remote_ctx);
  remote_list->apply(insertOp);
  ASSERT_EQ(remote_list->size(), 1);
  std::vector<char> expectedElements{'a'};
  ASSERT_EQ(remote_list->getContents(), expectedElements);
}

TEST_F(ListTest, ListRemoteDeleteTest) {
  // get the operation from a local insert
  const nupad::crdt::InsertOperation insertOp = list_->insert(0, 'a');
  const nupad::crdt::InsertOperation insertOp2 = list_->insert(0, 'b');
  const nupad::crdt::DeleteOperation deleteOp = list_->remove(1);
  nupad::PeerId remotePeerId = "2";
  nupad::Context remote_ctx(remotePeerId, nupad::clock::VectorClock
      (remotePeerId));
  auto remote_list = new nupad::crdt::DoublyLinkedList<char>(remote_ctx);
  // apply in the same order as above
  remote_list->apply(insertOp);
  remote_list->apply(insertOp2);
  remote_list->apply(deleteOp);
  ASSERT_EQ(remote_list->size(), 1);
  std::vector<char> expectedElements{'b'};
  ASSERT_EQ(remote_list->getContents(), expectedElements);
}

TEST_F(ListTest, ListInsertSameIndexTest) {
  list_->insert(0, 'a');
  list_->insert(1, 'b');
  list_->insert(1, 'c');
  ASSERT_EQ(list_->size(), 3);
  std::vector<char> expectedElements{'a', 'c', 'b'};
  ASSERT_EQ(list_->getContents(), expectedElements);
}
