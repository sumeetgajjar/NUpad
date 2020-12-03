//
// Created by sumeet on 11/18/20.
//

#ifndef NUPAD_LIST_H
#define NUPAD_LIST_H

#include <list>
#include <glog/logging.h>
#include <functional>
#include <unordered_set>
#include <utility>

#include "operation.h"
#include "common.pb.h"

namespace nupad::crdt {

    class DoublyLinkedList {
        class Node {
        public:
            const common::ID insertionTS;
            const std::string value;
            Node *prev = nullptr, *next = nullptr;
            std::optional<common::ID> deletionTS;

            Node(common::ID insertionTS, std::string value, Node *prev, Node *next) :
                    insertionTS(std::move(insertionTS)), value(std::move(value)), prev(prev), next(next),
                    deletionTS(std::nullopt) {}
        };

        std::unordered_map<common::ID, Node *, common::IDHash> elementIdToNodeMap_;
        Node *head_ = nullptr, *tail_ = nullptr;
        const std::function<common::ID(void)> nextElementIdGetter_;
        size_t size_ = 0;
        std::unordered_set<common::ID, common::IDHash> appliedOperations_;


        void applyInsertOperation(const common::Operation &operation);

        void applyDeleteOperation(const common::Operation &operation);

        Node *getNodeByIndex(int index);

        Node *insertAfterId(const std::optional<common::ID> &insertAfterElementId,
                            const common::ID &newElementId, const std::string &value);

    public:

        explicit DoublyLinkedList(std::function<common::ID(void)> nextElementIdGetter);

        void apply(const common::Operation &operation);

        void insert(int index, char value, common::Operation *opPtr);

        void insert(int index, const std::string &value, common::Operation *opPtr);

        void remove(int index, common::Operation *opPtr);

        std::string getContents();

        size_t size() const;
    };
}

#endif //NUPAD_LIST_H
