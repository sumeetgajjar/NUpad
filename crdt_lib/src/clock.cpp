//
// Created by sumeet on 11/18/20.
//

#include "clock.h"

namespace nupad::clock {

    VectorClock::VectorClock(PeerId myPeerId) : myPeerId_(std::move(myPeerId)) {
        CHECK_NE(myPeerId_, "") << "Peer ID cannot be empty";
        clockState_[myPeerId_] = 0;
        LOG(INFO) << "VectorClock initialized";
    }

    Tick VectorClock::tick() {
        clockState_.at(myPeerId_)++;
        return clockState_.at(myPeerId_);
    }

    void VectorClock::update(const PeerId &otherPeerId, const Tick tick) {
        CHECK_NE(myPeerId_, otherPeerId) << "Cannot update my own tick";
        if(clockState_.find(otherPeerId) != clockState_.end()) {
           CHECK_LT(clockState_.at(otherPeerId), tick)
            << "new tick value should be strictly greater than current tick value";
        }
        clockState_[otherPeerId] = tick;
    }

    Tick VectorClock::getTick(const PeerId &peerId) const {
        CHECK_NE(peerId, "") << "Peer ID cannot be empty";
        return clockState_.at(peerId);
    }

    Tick VectorClock::getMyTick() const {
        return clockState_.at(myPeerId_);
    }

    ClockState VectorClock::getState() const {
        return ClockState(clockState_);
    }
}