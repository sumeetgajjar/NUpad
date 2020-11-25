//
// Created by sumeet on 11/18/20.
//

#include "clock.h"

#include <glog/logging.h>

namespace nupad::clock {

    PeerId VectorClock::myPeerId_;
    bool VectorClock::initialized_ = false;
    ClockState VectorClock::clockState_(/*initial capacity*/ 10);

    void VectorClock::init(PeerId myPeerId) {
        CHECK(!VectorClock::initialized_) << "VectorClock already initialized";
        myPeerId_ = std::move(myPeerId);
        CHECK_STRNE(myPeerId_.c_str(), "") << "myPeerId cannot be empty";
        clockState_[myPeerId_] = 0;
        initialized_ = true;
        LOG(INFO) << "VectorClock initialized";
    }

    Tick VectorClock::tick(const Tick tick) {
        CHECK(initialized_) << "VectorClock not initialized";
        clockState_.at(myPeerId_) += tick;
        return clockState_.at(myPeerId_);
    }

    void VectorClock::update(const PeerId &peerId, const Tick tick) {
        CHECK(initialized_) << "VectorClock not initialized";
        if (clockState_.find(peerId) != clockState_.end()) {
            CHECK_LT(clockState_.at(peerId), tick)
                << "new tick value should be strictly greater than current tick value";
        }
        clockState_[peerId] = tick;
    }

    Tick VectorClock::getTick(const PeerId &peerId) {
        CHECK(initialized_) << "VectorClock not initialized";
        return clockState_.at(peerId);
    }

    ClockState VectorClock::getState() {
        CHECK(initialized_) << "VectorClock not initialized";
        return ClockState(clockState_);
    }
}