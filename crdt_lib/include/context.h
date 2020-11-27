//
// Created by sumeet on 11/23/20.
//

#ifndef NUPAD_CONTEXT_H
#define NUPAD_CONTEXT_H

#include "clock.h"
#include "operation.h"

namespace nupad {

    class Context {
        const PeerId myPeerId_;
        clock::VectorClock clock_;
    public:
        explicit Context(PeerId myPeerId);

        crdt::ElementId getNextElementId();
    };
}

#endif //NUPAD_CONTEXT_H
