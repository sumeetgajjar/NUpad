//
// Created by sumeet on 11/23/20.
//

#ifndef NUPAD_DOCUMENT_H
#define NUPAD_DOCUMENT_H

#include <unordered_map>
#include <vector>
#include <deque>
#include <memory>
#include "list.h"
#include "types.h"
#include "context.h"
#include "operation.h"

namespace nupad {

    class Change {
        PeerId peerId_;
        crdt::ElementId timestamp_;
        int changeCount_;
        std::deque<std::shared_ptr<crdt::Operation>> operations_;
//        TODO: include the remote clock update operation in change as dependencies
    public:
        Change(PeerId peerId, crdt::ElementId timestamp, int changeCount,
               const std::deque<std::shared_ptr<crdt::Operation>> &operations);

        Change(Change &&other);

        Change &operator=(Change &&other) noexcept;

        const PeerId &getPeerId() const;

        const crdt::ElementId &getTimestamp() const;

        int getChangeCount() const;

        std::deque<std::shared_ptr<crdt::Operation>> &getOperations();
    };

    class Document {
        const PeerId peerId_;
        const std::string name_;
        clock::VectorClock clock_;
        clock::VectorClock localClockUpdate_;
        clock::VectorClock nextTimestampByPeerId_;
        crdt::DoublyLinkedList<char> list_;

        std::deque<std::shared_ptr<crdt::Operation>> operations_;
        std::unordered_map<PeerId, std::vector<Change>> receiveBuffer_;
        std::unordered_map<PeerId, clock::VectorClock> changeCounters_;

        void addClockUpdateToChanges();

        crdt::ElementId getNextElementId();

        std::optional<PeerId> getCasuallyReadyPeer();

        bool isCausallyReady(const PeerId &peerId);

        void applyCasuallyReadyChanges();

        void applyRemotePeerChanges(const PeerId &remotePeerId);

        void applyOperation(const crdt::Operation &operation);

        void applyClockUpdateOperation(const crdt::ClockUpdateOperation *clockUpdateOperation);

        void applyChangeProcessedOperation(const crdt::ChangeProcessedOperation *changeProcessedOperation);

    public:
        Document(PeerId peerId, std::string name);

        void insert(int index, char value);

        void remove(int index);

        bool hasChange();

        Change getChange();

        void processChange(Change &change);
    };
}

#endif //NUPAD_DOCUMENT_H
