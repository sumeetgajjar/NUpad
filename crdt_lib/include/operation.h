//
// Created by sumeet on 11/23/20.
//

#ifndef NUPAD_OPERATION_H
#define NUPAD_OPERATION_H

#include <ostream>

#include "types.h"
#include "common.pb.h"

namespace nupad::common {
    bool operator<(const ID &lhs, const ID &rhs);

    bool operator>(const ID &lhs, const ID &rhs);

    bool operator<=(const ID &lhs, const ID &rhs);

    bool operator>=(const ID &lhs, const ID &rhs);

    bool operator==(const ID &lhs, const ID &rhs);

    bool operator!=(const ID &lhs, const ID &rhs);

    class IDHash {
    public :
        std::size_t operator()(const ID &elementId) const;
    };

    std::ostream &operator<<(std::ostream &os, const ID &id);

    std::ostream &operator<<(std::ostream &os, const OperationType &operationType);
}
#endif //NUPAD_OPERATION_H
