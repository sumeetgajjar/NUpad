//
// Created by sumeet on 11/23/20.
//

#ifndef NUPAD_OPERATION_H
#define NUPAD_OPERATION_H

#include <ostream>

#include "types.h"

namespace nupad::crdt {
    enum OperationType {
        Insert = 1,
        Delete = 2,
        ClockUpdate = 3,
        ChangeProcessed = 4
    };

    std::ostream &operator<<(std::ostream &os, const OperationType &operationType);

    class ElementId {
        PeerId peerId_;
        nupad::clock::Tick tick_;

    public:
        ElementId(PeerId peerId, const clock::Tick &tick);

        ElementId(const ElementId &other);

        ElementId(ElementId &&other);

        const PeerId &getPeerId() const;

        clock::Tick getTick() const;

        ElementId &operator=(const ElementId &other);

        ElementId &operator=(ElementId &&other) noexcept;

        bool operator<(const ElementId &rhs) const;

        bool operator>(const ElementId &rhs) const;

        bool operator<=(const ElementId &rhs) const;

        bool operator>=(const ElementId &rhs) const;

        bool operator==(const ElementId &rhs) const;

        bool operator!=(const ElementId &rhs) const;

        friend std::ostream &operator<<(std::ostream &os, const ElementId &id);
    };

    class ElementIdHash {
    public :
        std::size_t operator()(const ElementId &elementId) const {
            std::size_t peerIdHash = std::hash<PeerId>()(elementId.getPeerId());
            std::size_t tickHash = std::hash<clock::Tick>()(elementId.getTick());
            return peerIdHash ^ (tickHash << 1);
        }
    };

    class Operation {
    protected:
        const OperationType operationType_;

        explicit Operation(OperationType operationType);

    public:
        OperationType getOperationType() const;

        virtual std::optional<ElementId> getTimeStamp() const;

        friend std::ostream &operator<<(std::ostream &os, const Operation &operation);
    };


    template<typename T>
    class InsertOperation : public Operation {
        const ElementId newElementId_;
        const T value_;
        // Element after which this element should be inserted.
        const std::optional<ElementId> prevElementId_;
    public:

        InsertOperation(ElementId elementId, const T value, std::optional<ElementId> prevElementId) :
                Operation(Insert),
                newElementId_(std::move(elementId)),
                value_(value),
                prevElementId_(std::move(prevElementId)) {}

        const ElementId &getNewElementId() const {
            return newElementId_;
        }

        const T getValue() const {
            return value_;
        }

        const std::optional<ElementId> &getPrevElementId() const {
            return prevElementId_;
        }

        std::optional<ElementId> getTimeStamp() const override {
            return newElementId_;
        }
    };

    class DeleteOperation : public Operation {
        const ElementId elementId_;
        const ElementId deleteTS_;
    public:
        DeleteOperation(ElementId elementId, ElementId deleteTS);

        const ElementId &getElementId() const;

        const ElementId &getDeleteTS() const;

        std::optional<ElementId> getTimeStamp() const override;
    };

    class ClockUpdateOperation : public Operation {

        const clock::Tick nextTimestamp_;
        const clock::ClockState diff;
    public:
        ClockUpdateOperation(clock::Tick nextTimestamp, clock::ClockState diff);

        std::optional<ElementId> getTimeStamp() const override;
    };

    class ChangeProcessedOperation : public Operation {
        const int changeCounter;
    public:
        ChangeProcessedOperation(int changeCounter);

        std::optional<ElementId> getTimeStamp() const override;
    };
}

#endif //NUPAD_OPERATION_H
