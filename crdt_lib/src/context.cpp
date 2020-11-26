//
// Created by sumeet on 11/23/20.
//
#include "clock.h"
#include "context.h"

#include <utility>

namespace nupad {

Context::Context(PeerId myPeerId, clock::VectorClock clock)
    : myPeerId(std::move(myPeerId)), clock_(std::move(clock)) {
    }

    crdt::ElementId Context::getNextElementId() {
        return crdt::ElementId(myPeerId, clock_.tick());
    }
}