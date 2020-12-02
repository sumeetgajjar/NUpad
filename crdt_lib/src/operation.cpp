//
// Created by sumeet on 11/23/20.
//

#include "operation.h"
#include <glog/logging.h>

namespace nupad::common {

    bool operator<(const ID &lhs, const ID &rhs) {
        if (lhs.tick().value() < rhs.tick().value()) {
            return true;
        }
        if (rhs.tick().value() < lhs.tick().value()) {
            return false;
        }
        return lhs.peer_id() < rhs.peer_id();
    }

    bool operator>(const ID &lhs, const ID &rhs) {
        return rhs < lhs;
    }

    bool operator<=(const ID &lhs, const ID &rhs) {
        return !(rhs < lhs);
    }

    bool operator>=(const ID &lhs, const ID &rhs) {
        return !(lhs < rhs);
    }

    bool operator==(const ID &lhs, const ID &rhs) {
        return lhs.peer_id() == rhs.peer_id() && lhs.tick().value() == rhs.tick().value();
    }

    bool operator!=(const ID &lhs, const ID &rhs) {
        return !(rhs == lhs);
    }

    std::size_t IDHash::operator()(const ID &elementId) const {
        std::size_t peerIdHash = std::hash<std::string>()(elementId.peer_id());
        std::size_t tickHash = std::hash<int64_t>()(elementId.tick().value());
        return peerIdHash ^ (tickHash << 1);
    }

    std::ostream &operator<<(std::ostream &os, const ID &id) {
        os << "peerId: " << id.peer_id() << ", Tick: " << id.tick().value();
        return os;
    }

    std::ostream &operator<<(std::ostream &os, const OperationType &operationType) {
        switch (operationType) {
            case OperationType::INSERT:
                os << "OperationType: Insert";
                break;
            case OperationType::DELETE:
                os << "OperationType: Delete";
                break;
            default:
                LOG(FATAL) << "Unknown OperationType encountered";
        }
        return os;
    }
}