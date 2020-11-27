//
// Created by madhurjain on 11/26/20.
//
#include "context.h"
#include <gtest/gtest.h>

class ContextTestSuite : public ::testing::Test {
protected:
  void SetUp() override {
    myPeerId_ = "1";
    ctx_ = new nupad::Context(myPeerId_);
  }

  void TearDown() override {
    delete ctx_;
  }

  nupad::PeerId myPeerId_;
  nupad::Context *ctx_;
};

TEST_F(ContextTestSuite, ContextInitTest) {
    nupad::crdt::ElementId expectedElementId{"1", 1};
    ASSERT_EQ(ctx_->getNextElementId(), expectedElementId);
}