//
// Created by sumeet on 12/1/20.
//

#include "test_utils.h"
#include "document.h"
#include "gtest/gtest.h"

class DocumentTestSuite : public ::testing::Test {
protected:
    void SetUp() override {
        docSize_ = 100;
        doc1_ = new nupad::Document("1", "doc-1");
        doc2_ = new nupad::Document("2", "doc-2");
        doc3_ = new nupad::Document("3", "doc-3");
        doc4_ = new nupad::Document("4", "doc-4");
    }

    void TearDown() override {
        delete doc1_;
        delete doc2_;
        delete doc3_;
        delete doc4_;
    }

    nupad::Document *doc1_;
    nupad::Document *doc2_;
    nupad::Document *doc3_;
    nupad::Document *doc4_;
    int docSize_;

    static std::string insertRandomCharacter(nupad::Document &doc, int count);
};

std::string DocumentTestSuite::insertRandomCharacter(nupad::Document &doc, int count) {
    std::string expectedStr;
    for (int i = 0; i < count; ++i) {
        char value = TestUtils::getRandomChar();
        doc.insert(i, value);
        expectedStr += value;
    }
    return expectedStr;
}

TEST_F(DocumentTestSuite, TestEmptyInit) {
    ASSERT_FALSE(doc1_->hasChange());
    ASSERT_EQ(doc1_->getString(), "");
    ASSERT_DEATH(doc1_->getChange(), "getChange called when no document has no changes");
}

TEST_F(DocumentTestSuite, TestInsert) {
    std::string expectedStr = insertRandomCharacter(*doc1_, docSize_);

    ASSERT_EQ(doc1_->size(), docSize_);
    ASSERT_EQ(doc1_->getString(), expectedStr);
    ASSERT_TRUE(doc1_->hasChange());

    auto change = doc1_->getChange();
    ASSERT_EQ(change.operations_size(), docSize_);

    ASSERT_FALSE(doc1_->hasChange());
    ASSERT_DEATH(doc1_->getChange(), "getChange called when no document has no changes");
}

TEST_F(DocumentTestSuite, TestDelete) {
    std::string expectedStr = insertRandomCharacter(*doc1_, docSize_);

    int delCount = TestUtils::getRandomInt(0, docSize_ / 2);
    for (int i = 0; i < delCount; ++i) {
        doc1_->remove(0);
    }
    ASSERT_EQ(doc1_->size(), docSize_ - delCount);
    ASSERT_EQ(doc1_->getString(), expectedStr.substr(delCount));

    ASSERT_TRUE(doc1_->hasChange());
    auto change = doc1_->getChange();
    ASSERT_EQ(change.operations_size(), docSize_ + delCount);

    ASSERT_FALSE(doc1_->hasChange());
    ASSERT_DEATH(doc1_->getChange(), "getChange called when no document has no changes");
}

TEST_F(DocumentTestSuite, TestGetChange) {
    int changeCount = 100;
    for (int i = 0; i < changeCount; ++i) {
        std::string expectedStr = insertRandomCharacter(*doc1_, 1);
        auto change = doc1_->getChange();

        nupad::common::ID expectedChangeId;
        expectedChangeId.set_peer_id("1");
        expectedChangeId.mutable_tick()->set_value(i + 1);

        ASSERT_EQ(change.change_id(), expectedChangeId);
        ASSERT_EQ(change.operations_size(), 1);
    }
}

TEST_F(DocumentTestSuite, TestSelfApplyFails) {
    std::string expectedStr = insertRandomCharacter(*doc1_, docSize_);
    auto doc1Change = doc1_->getChange();
    ASSERT_DEATH(doc1_->processChange(doc1Change), "cannot apply my change");
}

TEST_F(DocumentTestSuite, TestApplyInsert) {
    std::string expectedStr = insertRandomCharacter(*doc1_, docSize_);
    auto doc1Change = doc1_->getChange();
    doc2_->processChange(doc1Change);

    ASSERT_EQ(doc2_->getString(), expectedStr);
    ASSERT_EQ(doc2_->size(), docSize_);
}

TEST_F(DocumentTestSuite, TestApplyDelete) {
    std::string expectedStr = insertRandomCharacter(*doc1_, docSize_);
    int delCount = docSize_ / 2;
    for (int i = 0; i < delCount; ++i) {
        doc1_->remove(0);
    }

    auto doc1Change = doc1_->getChange();
    doc2_->processChange(doc1Change);

    ASSERT_EQ(doc2_->getString(), expectedStr.substr(delCount));
    ASSERT_EQ(doc2_->size(), docSize_ - delCount);
}

TEST_F(DocumentTestSuite, TestApplyCausal) {
    std::string expectedStr = insertRandomCharacter(*doc1_, docSize_);
    auto doc1InsertChange = doc1_->getChange();

    int delCount = docSize_ / 2;
    for (int i = 0; i < delCount; ++i) {
        doc1_->remove(0);
    }

    auto doc1DeleteChange = doc1_->getChange();
    doc2_->processChange(doc1DeleteChange);
    ASSERT_EQ(doc2_->getString(), "");
    ASSERT_EQ(doc2_->size(), 0);

    doc2_->processChange(doc1InsertChange);
    ASSERT_EQ(doc2_->getString(), expectedStr.substr(delCount));
    ASSERT_EQ(doc2_->size(), docSize_ - delCount);
}

TEST_F(DocumentTestSuite, TestDuplicateApply) {
    std::string expectedStr = insertRandomCharacter(*doc1_, docSize_);
    auto doc1Change = doc1_->getChange();

    for (int i = 0; i < 10; ++i) {
        doc2_->processChange(doc1Change);
        ASSERT_EQ(doc2_->getString(), expectedStr);
        ASSERT_EQ(doc2_->size(), docSize_);
    }
}

TEST_F(DocumentTestSuite, TestOutOfOrderApply) {
    std::vector<nupad::common::Change> changes;
    int changeCount = 10;
    for (int i = 0; i < changeCount; ++i) {
        std::string expectedStr = insertRandomCharacter(*doc1_, docSize_);
        changes.push_back(doc1_->getChange());
    }

    for (int i = changeCount - 1; i >= 0; i--) {
        doc2_->processChange(changes[i]);
    }

    ASSERT_EQ(doc1_->size(), doc2_->size());
    ASSERT_EQ(doc1_->getString(), doc2_->getString());
}

TEST_F(DocumentTestSuite, TestRandomApply) {
    std::vector<nupad::common::Change> changes;
    int changeCount = 1000;
    for (int i = 0; i < changeCount; ++i) {
        std::string expectedStr = insertRandomCharacter(*doc1_, docSize_);
        changes.push_back(doc1_->getChange());
    }

    std::shuffle(changes.begin(), changes.end(), TestUtils::getRandomEngine());

    for (int i = changeCount - 1; i >= 0; i--) {
        doc2_->processChange(changes[i]);
    }

    ASSERT_EQ(doc1_->size(), doc2_->size());
    ASSERT_EQ(doc1_->getString(), doc2_->getString());
}

TEST_F(DocumentTestSuite, TestConcurrentInsert) {
    docSize_ = 3;
    std::string str1 = "abc";
    for (size_t i = 0; i < str1.size(); i++) {
        doc1_->insert(i, str1[i]);
    }
    auto doc1Change = doc1_->getChange();
    doc2_->processChange(doc1Change);

    std::string str2 = "def";
    for (size_t i = 0; i < str2.size(); ++i) {
        doc2_->insert(1, str2[i]);
    }

    auto doc2Change = doc2_->getChange();
    doc1_->processChange(doc2Change);

    std::string expectedStr = "afedbc";
    ASSERT_EQ(doc1_->getString(), expectedStr);
    ASSERT_EQ(doc1_->size(), expectedStr.size());

    ASSERT_EQ(doc1_->size(), doc2_->size());
    ASSERT_EQ(doc1_->getString(), doc2_->getString());
}


TEST_F(DocumentTestSuite, TestConcurrentDelete) {
    doc1_->insert(0, 'a');
    auto doc1Change = doc1_->getChange();
    doc2_->processChange(doc1Change);

    doc1_->remove(0);
    doc2_->remove(0);

    auto doc2Change = doc2_->getChange();
    doc1_->processChange(doc2Change);
    doc1Change = doc1_->getChange();
    doc2_->processChange(doc1Change);

    ASSERT_EQ(doc1_->size(), 0);
    ASSERT_EQ(doc2_->size(), 0);
    ASSERT_EQ(doc1_->getString(), "");
    ASSERT_EQ(doc2_->getString(), "");
}













