//
// Created by sumeet on 11/23/20.
//

#ifndef NUPAD_OPERATION_H
#define NUPAD_OPERATION_H

#include <utility>
#include <ostream>

#include "types.h"

namespace nupad::crdt {
    enum OperationType {
        Insert = 1,
        Delete = 2,
        ClockUpdate = 3
    };

    std::ostream &operator<<(std::ostream &os, const OperationType &operationType);

    class ElementId {
        const PeerId peerId_;
        const nupad::clock::Tick tick_;

    public:
        ElementId(PeerId peerId, const clock::Tick &tick);

        const PeerId &getPeerId() const;

        const clock::Tick getTick() const;

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

        virtual ~Operation() = default;

    public:
        OperationType getOperationType() const;

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
                prevElementId_(std::move(prevElementId)),
                newElementId_(std::move(elementId)),
                value_(value) {}

        const ElementId &getNewElementId() const {
            return newElementId_;
        }

        const T getValue() const {
            return value_;
        }

        const std::optional<ElementId> &getPrevElementId() const {
            return prevElementId_;
        }
    };

    class DeleteOperation : public Operation {
        const ElementId elementId_;
        const ElementId deleteTS_;
    public:
        DeleteOperation(ElementId elementId, ElementId deleteTS);

        const ElementId &getElementId() const;

        const ElementId &getDeleteTS() const;
    };

    class ClockUpdateOperation : public Operation {

    };
}

#endif //NUPAD_OPERATION_H
