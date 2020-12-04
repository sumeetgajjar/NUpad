//
// Created by sumeet on 12/1/20.
//

#include "list.h"

namespace nupad::crdt {
    DoublyLinkedList::DoublyLinkedList(std::function<common::ID(void)> nextElementIdGetter) :
            nextElementIdGetter_(std::move(nextElementIdGetter)) {
        LOG(INFO) << "DoublyLinkedList initialized";
    }

    void DoublyLinkedList::applyInsertOperation(const common::Operation &operation) {
        CHECK(operation.has_insert_operation());
        CHECK(operation.insert_operation().has_new_element_id());
        CHECK_NE(operation.insert_operation().value(), "");
        insertAfterId(operation.insert_operation().has_prev_element_id() ?
                      operation.insert_operation().prev_element_id() : std::optional<common::ID>(),
                      operation.insert_operation().new_element_id(),
                      operation.insert_operation().value());
    }

    void DoublyLinkedList::applyDeleteOperation(const common::Operation &operation) {
        CHECK(operation.has_delete_operation());
        CHECK(operation.delete_operation().has_element_id());
        CHECK(operation.delete_operation().has_delete_timestamp());

        const auto &elementIdToDelete = operation.delete_operation().element_id();
        CHECK(elementIdToNodeMap_.find(elementIdToDelete) != elementIdToNodeMap_.end())
                        << "elementID: " << elementIdToDelete << " not exists in the list";
        Node *deleteNode = elementIdToNodeMap_.at(operation.delete_operation().element_id());

        // Deleting only if node is not deleted
        // A given node can be queued for multiple delete via applyOperation
        if (!deleteNode->deletionTS.has_value()) {
            deleteNode->deletionTS.emplace(operation.delete_operation().delete_timestamp());
            size_--;
        }
    }

    DoublyLinkedList::Node *DoublyLinkedList::getNodeByIndex(const int index) {
        Node *node = head_;
        int distance = index;
        while (node != nullptr && (node->deletionTS.has_value() || distance > 0)) {
            if (!node->deletionTS.has_value()) {
                distance--;
            }
            node = node->next;
        }

        // If any node is found, then the distance should be zero.
        // A node will not be found when inserting at the end of the list and
        // the following checking should be ignored
        if (node != nullptr) {
            CHECK_EQ(distance, 0) << "could not get to index:" << index;
        }
        return node;
    }

    DoublyLinkedList::Node *DoublyLinkedList::insertAfterId(const std::optional<common::ID> &insertAfterElementId,
                                                            const common::ID &newElementId, const std::string &value) {
        CHECK(elementIdToNodeMap_.find(newElementId) == elementIdToNodeMap_.end())
                        << "elementID: " << newElementId << " already exists in the list";
        Node *prev = nullptr;
        if (insertAfterElementId.has_value()) {
            prev = elementIdToNodeMap_.at(insertAfterElementId.value());
        } else if (head_ != nullptr && head_->insertionTS > newElementId) {
            prev = head_;
        }

        while (prev != nullptr && prev->next != nullptr && prev->next->insertionTS > newElementId) {
            prev = prev->next;
        }

        Node *next = prev != nullptr ? prev->next : head_;
        Node *newNode = new Node(newElementId, value, prev, next);
        elementIdToNodeMap_[newElementId] = newNode;
        if (prev != nullptr) {
            prev->next = newNode;
        }
        if (next != nullptr) {
            next->prev = newNode;
        }
        if (prev == nullptr) {
            head_ = newNode;
        }
        if (next == nullptr) {
            tail_ = newNode;
        }

        size_++;
        return newNode;
    }

    void DoublyLinkedList::apply(const common::Operation &operation) {
        // TODO: test this
        CHECK(operation.has_operation_id());
        CHECK_NE(operation.operation_type(), common::OperationType::NONE);
        CHECK(appliedOperations_.find(operation.operation_id()) == appliedOperations_.end())
                        << "Duplicate operation, operationId: " << operation.has_operation_id()
                        << ", operationType: " << operation.operation_type();

        appliedOperations_.insert(operation.operation_id());
        switch (operation.operation_type()) {
            case common::OperationType::INSERT:
                applyInsertOperation(operation);
                break;
            case common::OperationType::DELETE:
                applyDeleteOperation(operation);
                break;
            default:
                LOG(FATAL) << "unexpected Operation: " << operation.operation_type();
        }

    }

    void DoublyLinkedList::insert(const int index, const char value, common::Operation *opPtr) {
        return insert(index, std::string{value}, opPtr);
    }

    void DoublyLinkedList::insert(const int index, const std::string &value, common::Operation *opPtr) {
        CHECK_EQ(value.size(), 1) << "only single insertion is allowed";
        CHECK_GE(index, 0) << "index cannot be negative: " << index;
        CHECK_LE(index, size_) << "index cannot be greater than the size "
                                  "of the list: " << index;

        std::optional<common::ID> prevElementId = std::nullopt;
        if (index > 0) {
            Node *next = getNodeByIndex(index);
            prevElementId.emplace(next != nullptr ? next->prev->insertionTS : tail_->insertionTS);
        }

        Node *inserted = insertAfterId(prevElementId, nextElementIdGetter_(), value);

        std::optional<common::ID> newNodePrevElementId = std::nullopt;
        if (inserted->prev != nullptr) {
            newNodePrevElementId.emplace(inserted->prev->insertionTS);
        }

        auto *mutableOperationId = opPtr->mutable_operation_id();
        mutableOperationId->set_peer_id(inserted->insertionTS.peer_id());
        mutableOperationId->mutable_tick()->set_value(inserted->insertionTS.tick().value());

        auto *mutableTimestamp = opPtr->mutable_timestamp();
        mutableTimestamp->set_peer_id(inserted->insertionTS.peer_id());
        mutableTimestamp->mutable_tick()->set_value(inserted->insertionTS.tick().value());

        opPtr->set_operation_type(common::OperationType::INSERT);

        auto *mutableInsertOp = opPtr->mutable_insert_operation();
        mutableInsertOp->mutable_new_element_id()->set_peer_id(inserted->insertionTS.peer_id());
        mutableInsertOp->mutable_new_element_id()->mutable_tick()->set_value(inserted->insertionTS.tick().value());

        mutableInsertOp->set_value(inserted->value);
        if (newNodePrevElementId.has_value()) {
            mutableInsertOp->mutable_prev_element_id()->set_peer_id(newNodePrevElementId->peer_id());
            mutableInsertOp->mutable_prev_element_id()->mutable_tick()->set_value(
                    newNodePrevElementId->tick().value());
        }
    }

    void DoublyLinkedList::remove(const int index, common::Operation *opPtr) {
        CHECK_GE(index, 0) << "index cannot be negative: " << index;
        CHECK_LE(index, size_) << "index cannot be greater than the size "
                                  "of the list: " << index;
        Node *node = getNodeByIndex(index);
        CHECK_NOTNULL(node);

        auto deletionTS = nextElementIdGetter_();
        auto *mutableOperationId = opPtr->mutable_operation_id();
        mutableOperationId->set_peer_id(deletionTS.peer_id());
        mutableOperationId->mutable_tick()->set_value(deletionTS.tick().value());

        auto *mutableTimestamp = opPtr->mutable_timestamp();
        mutableTimestamp->set_peer_id(deletionTS.peer_id());
        mutableTimestamp->mutable_tick()->set_value(deletionTS.tick().value());

        opPtr->set_operation_type(common::OperationType::DELETE);

        auto *mutableDeleteOp = opPtr->mutable_delete_operation();
        mutableDeleteOp->mutable_element_id()->set_peer_id(node->insertionTS.peer_id());
        mutableDeleteOp->mutable_element_id()->mutable_tick()->set_value(node->insertionTS.tick().value());

        mutableDeleteOp->mutable_delete_timestamp()->set_peer_id(deletionTS.peer_id());
        mutableDeleteOp->mutable_delete_timestamp()->mutable_tick()->set_value(deletionTS.tick().value());

        applyDeleteOperation(*opPtr);
    }

    std::string DoublyLinkedList::getContents() {
        std::string content;
        content.reserve(size_);

        Node *node = head_;
        while (node != nullptr) {
            if (!node->deletionTS.has_value()) {
                content += node->value;
            }
            node = node->next;
        }

        CHECK_EQ(content.size(), size_) << "list size mismatch";
        return content;
    }

    size_t DoublyLinkedList::size() const {
        return size_;
    }
}
