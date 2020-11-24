//
// Created by sumeet on 11/18/20.
//

#ifndef NUPAD_LIST_H
#define NUPAD_LIST_H

#include <list>
#include <utility>
#include <glog/logging.h>

#include "context.h"
#include "operation.h"

namespace nupad::crdt {

    /* All definitions are in-place to avoid linker error, this is a standard practice
     * used in STL.
     * ref: https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
     */
    template<typename T>
    class CRDTList {
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

        std::unordered_map<ElementId, Node *, ElementIdHash> elementIdToNodeMap;
        Node *head = nullptr, *tail = nullptr;
        Context context_;
        size_t size_ = 0;


        void applyInsertOperation(Operation &operation) {
            auto *insertOp = dynamic_cast<InsertOperation<T> *>(&operation);
            CHECK_NE(insertOp, nullptr) << "Cannot cast operation to InsertOperation: " << operation;
            insertAfterId(insertOp->getPrevElementId(), insertOp->getNewElementId(), insertOp->getValue());
        }

        void applyDeleteOperation(Operation &operation) {
            auto *deleteOperation = dynamic_cast<DeleteOperation *>(&operation);
            CHECK_NE(deleteOperation, nullptr) << "Cannot cast operation to DeleteOperation: " << operation;
            // TODO: implement this madhur;
        }

        Node *getNodeByIndex(const int index) {
            CHECK_GE(index, 0) << "index cannot be negative";
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

        void removeAfterId(const ElementId &elementId) {
            // TODO:// implement this madhur
        }

    public:
        CRDTList(const Context &context) : context_(context) {
            LOG(INFO) << "CRDTList of " << typeid(T).name() << " initialized";
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
            if (index > size_) {
                throw std::out_of_range("List size: " + std::to_string(size_) +
                                        ", insertion index: " + std::to_string(index) + " is out of range");
            }

            std::optional<ElementId> prevElementId = std::nullopt;
            if (index > 0) {
                Node *next = getNodeByIndex(index);
                prevElementId.emplace(next != nullptr ? next->prev->insertionTS : tail->insertionTS);
            }

            Node *inserted = insertAfterId(prevElementId, context_.getNextElementId(), value);

            std::optional<ElementId> newNodePrevElementId = std::nullopt;
            if (inserted->prev != nullptr) {
                newNodePrevElementId.emplace(inserted->prev->insertionTS);
            }

            return InsertOperation<T>(inserted->insertionTS, inserted->value, newNodePrevElementId);
        }

        DeleteOperation remove(const int index) {
            // TODO:// implement this madhur
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
