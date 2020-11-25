//
// Created by sumeet on 11/23/20.
//
#include "clock.h"
#include "context.h"

namespace nupad {

    Context::Context(PeerId myPeerId) : myPeerId(std::move(myPeerId)), clock_
                                    (myPeerId) {}

    crdt::ElementId Context::getNextElementId() {
        return crdt::ElementId(myPeerId, clock_.tick());
    }
}