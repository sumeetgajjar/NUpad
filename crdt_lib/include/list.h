//
// Created by sumeet on 11/18/20.
//

#ifndef NUPAD_LIST_H
#define NUPAD_LIST_H

#include <list>
#include <glog/logging.h>
#include <functional>

#include "context.h"
#include "operation.h"

namespace nupad::crdt {

    /* All definitions are in-place to avoid linker error, this is a standard practice
     * used in STL.
     * ref: https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
     */
    template<typename T>
    class DoublyLinkedList {
        class Node {
        public:
            const ElementId insertionTS;
            const T value;
            Node *prev = nullptr, *next = nullptr;
            std::optional<ElementId> deletionTS;

            Node(ElementId insertionTS, const T value, Node *prev, Node *next) :
                    insertionTS(std::move(insertionTS)), value(value), prev(prev), next(next),
                    deletionTS(std::nullopt) {}
        };

        // TODO: suffix all members with _
        std::unordered_map<ElementId, Node *, ElementIdHash> elementIdToNodeMap;
        Node *head = nullptr, *tail = nullptr;
        const std::function<ElementId(void)> nextElementIdGetter_;
        size_t size_ = 0;


        void applyInsertOperation(const Operation &operation) {
            auto *insertOp = dynamic_cast<const InsertOperation<T> *>(&operation);
            CHECK_NOTNULL(insertOp);
            insertAfterId(insertOp->getPrevElementId(), insertOp->getNewElementId(), insertOp->getValue());
        }

        void applyDeleteOperation(const Operation &operation) {
            auto *deleteOp = dynamic_cast<const DeleteOperation *>(&operation);
            CHECK_NOTNULL(deleteOp);
            CHECK(elementIdToNodeMap.find(deleteOp->getElementId()) !=
                  elementIdToNodeMap.end())
                            << "elementID: " << deleteOp->getElementId()
                            << " not exists in the list";
            Node *deleteNode = elementIdToNodeMap.at
                    (deleteOp->getElementId());
            deleteNode->deletionTS.emplace(deleteOp->getDeleteTS());
            size_--;
        }

        Node *getNodeByIndex(const int index) {
            CHECK_GE(index, 0) << "index cannot be negative: " << index;
            CHECK_LE(index, size_) << "index cannot be greater than the size "
                                      "of the list: " << index;
            Node *node = head;
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

        Node *insertAfterId(const std::optional<ElementId> &insertAfterElementId,
                            const ElementId &newElementId, const T value) {
            CHECK(elementIdToNodeMap.find(newElementId) == elementIdToNodeMap.end())
                            << "elementID: " << newElementId << " already exists in the list";
            Node *prev = nullptr;
            if (insertAfterElementId.has_value()) {
                prev = elementIdToNodeMap.at(insertAfterElementId.value());
            } else if (head != nullptr && head->insertionTS > newElementId) {
                prev = head;
            }

            while (prev != nullptr && prev->next != nullptr && prev->next->insertionTS > newElementId) {
                prev = prev->next;
            }

            Node *next = prev != nullptr ? prev->next : head;
            Node *newNode = new Node(newElementId, value, prev, next);
            elementIdToNodeMap[newElementId] = newNode;
            if (prev != nullptr) {
                prev->next = newNode;
            }
            if (next != nullptr) {
                next->prev = newNode;
            }
            if (prev == nullptr) {
                head = newNode;
            }
            if (next == nullptr) {
                tail = newNode;
            }

            size_++;
            return newNode;
        }

    public:
        // TODO: remove this constructor
        explicit DoublyLinkedList(Context &context) : nextElementIdGetter_(
                [&]() { return context.getNextElementId(); }) {
            LOG(INFO) << "DoublyLinkedList of " << typeid(T).name() << " initialized";
        }

        explicit DoublyLinkedList(std::function<ElementId(void)> nextElementIdGetter) :
                nextElementIdGetter_(std::move(nextElementIdGetter)) {
            LOG(INFO) << "DoublyLinkedList of " << typeid(T).name() << " initialized";
        }

        void apply(const Operation &operation) {
            switch (operation.getOperationType()) {
                case Insert:
                    applyInsertOperation(operation);
                    break;
                case Delete:
                    applyDeleteOperation(operation);
                    break;
                default:
                    LOG(FATAL) << "unexpected Operation: " << operation;
            }
        }

        InsertOperation<T> insert(const int index, const T &value) {
            std::optional<ElementId> prevElementId = std::nullopt;
            if (index > 0) {
                Node *next = getNodeByIndex(index);
                prevElementId.emplace(next != nullptr ? next->prev->insertionTS : tail->insertionTS);
            }

            Node *inserted = insertAfterId(prevElementId, nextElementIdGetter_(), value);

            std::optional<ElementId> newNodePrevElementId = std::nullopt;
            if (inserted->prev != nullptr) {
                newNodePrevElementId.emplace(inserted->prev->insertionTS);
            }

            return InsertOperation<T>(inserted->insertionTS, inserted->value, newNodePrevElementId);
        }

        DeleteOperation remove(const int index) {
            Node *node = getNodeByIndex(index);
            CHECK_NOTNULL(node);
            auto deleteOp = DeleteOperation(node->insertionTS, nextElementIdGetter_());
            applyDeleteOperation(deleteOp);
            return deleteOp;
        }

        std::vector<T> getContents() {
            std::vector<T> allContents;
            allContents.reserve(size_);

            Node *node = head;
            while (node != nullptr) {
                if (!node->deletionTS.has_value()) {
                    allContents.push_back(node->value);
                }
                node = node->next;
            }

            CHECK_EQ(allContents.size(), size_) << "list size mismatch";
            return allContents;
        }

        size_t size() {
            return size_;
        }
    };
}

#endif //NUPAD_LIST_H
