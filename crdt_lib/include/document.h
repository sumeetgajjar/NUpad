//
// Created by sumeet on 11/23/20.
//

#ifndef NUPAD_DOCUMENT_H
#define NUPAD_DOCUMENT_H

#include <unordered_map>
#include <vector>
#include <deque>
#include <memory>
#include <unordered_set>
#include "list.h"
#include "clock.h"
#include "types.h"
#include "common.pb.h"

namespace nupad {

    class Document {
        const PeerId peerId_;
        const std::string name_;
        clock::Tick logicalTS_;
        clock::VectorClock changeClock_;
        crdt::DoublyLinkedList list_;
        std::unordered_set<std::string> editors;

        std::unordered_set<common::ID, common::IDHash> appliedChangeIDs;

        common::Change *bufferedChange_;
        std::unordered_map<PeerId, std::deque<common::Change>> receiveBuffer_;

        common::ID getNextElementId();

        bool isCausallyReady(const common::Change &change);

        void applyAllCasuallyReadyChange();

        void applyChange(common::Change &change);

        bool isDuplicateChange(const common::ID &changeId);

    public:
        Document(PeerId peerId, std::string name);

        void insert(int index, char value);

        void insert(int index, const std::string &value);

        void remove(int index);

        std::string getString();

        size_t size();

        bool hasChange();

        common::Change getChange();

        void processChange(common::Change &change);

        std::string getName();

        std::unordered_set<std::string> getEditors();
    };
}

#endif //NUPAD_DOCUMENT_H
