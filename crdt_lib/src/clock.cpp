//
// Created by sumeet on 11/18/20.
//

#include "clock.h"
#include <glog/logging.h>

namespace nupad::clock {

    VectorClock::VectorClock(PeerId myPeerId) : myPeerId_(std::move(myPeerId)) {
        CHECK_NE(myPeerId_, "") << "Peer ID cannot be empty";
        clockState_[myPeerId_] = 0;
        LOG(INFO) << "VectorClock initialized";
    }

    Tick VectorClock::tick(const Tick tick) {
        CHECK_GT(tick, 0) << "tick should be greater than 0";
        clockState_.at(myPeerId_) += tick;
        return clockState_.at(myPeerId_);
    }

    bool VectorClock::hasTick(const PeerId &peerId) const {
        return clockState_.find(peerId) != clockState_.end();
    }

    void VectorClock::update(const PeerId &otherPeerId, const Tick tick) {
        CHECK_NE(myPeerId_, otherPeerId) << "Cannot update my own tick";
        if (clockState_.find(otherPeerId) != clockState_.end()) {
            CHECK_LT(clockState_.at(otherPeerId), tick)
                << "new tick value should be strictly greater than current tick value";
        }
        clockState_[otherPeerId] = tick;
    }

    Tick VectorClock::getTick(const PeerId &peerId) const {
        CHECK_NE(peerId, "") << "Peer ID cannot be empty";
        return hasTick(peerId) ? clockState_.at(peerId) : 0;
    }

    Tick VectorClock::getMyTick() const {
        return clockState_.at(myPeerId_);
    }

    ClockState VectorClock::getState() const {
        return ClockState(clockState_);
    }
}