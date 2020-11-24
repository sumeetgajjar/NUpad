//
// Created by sumeet on 11/23/20.
//

#include "operation.h"
#include <glog/logging.h>

#include <utility>

namespace nupad::crdt {

    std::ostream &operator<<(std::ostream &os, const OperationType &operationType) {
        switch (operationType) {
            case Insert:
                os << "OperationType: Insert";
                break;
            case Delete:
                os << "OperationType: Delete";
                break;
            case ClockUpdate:
                os << "OperationType: ClockUpdate";
                break;
            default:
                LOG(FATAL) << "Unknown OperationType encountered";
        }
        return os;
    }

    ElementId::ElementId(PeerId peerId, const nupad::clock::Tick &tick) :
            peerId_(std::move(peerId)), tick_(tick) {}

    bool ElementId::operator==(const ElementId &rhs) const {
        return peerId_ == rhs.peerId_ && tick_ == rhs.tick_;
    }

    bool ElementId::operator!=(const ElementId &rhs) const {
        return !(rhs == *this);
    }

    bool ElementId::operator<(const ElementId &rhs) const {
        if (tick_ < rhs.tick_) {
            return true;
        }
        if (rhs.tick_ < tick_) {
            return false;
        }
        return peerId_ < rhs.peerId_;
    }

    bool ElementId::operator>(const ElementId &rhs) const {
        return rhs < *this;
    }

    bool ElementId::operator<=(const ElementId &rhs) const {
        return !(rhs < *this);
    }

    bool ElementId::operator>=(const ElementId &rhs) const {
        return !(*this < rhs);
    }

    const PeerId &ElementId::getPeerId() const {
        return peerId_;
    }

    const clock::Tick ElementId::getTick() const {
        return tick_;
    }

    std::ostream &operator<<(std::ostream &os, const ElementId &id) {
        os << "peerId_: " << id.peerId_ << " tick_: " << id.tick_;
        return os;
    }

    Operation::Operation(OperationType operationType) :
            operationType_(operationType) {}

    OperationType Operation::getOperationType() const {
        return operationType_;
    }

    std::ostream &operator<<(std::ostream &os, const Operation &operation) {
        os << "operationType: " << operation.operationType_;
        return os;
    }

    DeleteOperation::DeleteOperation(ElementId elementId, ElementId deleteTS)
            : Operation(Delete), elementId_(std::move(elementId)), deleteTS_(std::move(deleteTS)) {}

    const ElementId &DeleteOperation::getElementId() const {
        return elementId_;
    }

    const ElementId &DeleteOperation::getDeleteTS() const {
        return deleteTS_;
    }
}