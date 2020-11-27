//
// Created by sumeet on 11/23/20.
//
#include "clock.h"
#include "context.h"
#include <glog/logging.h>

#include <utility>

namespace nupad {

    Context::Context(PeerId myPeerId)
        : myPeerId_(std::move(myPeerId)), clock_(myPeerId_) {
        LOG(INFO) << "Context for PeerID: " << myPeerId_ << " initialized";
    }

    crdt::ElementId Context::getNextElementId() {
        return crdt::ElementId(myPeerId_, clock_.tick());
    }
}