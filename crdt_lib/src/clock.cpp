//
// Created by sumeet on 11/18/20.
//

#include "clock.h"

namespace nupad::clock {

    VectorClock::VectorClock(PeerId myPeerId) : myPeerId_(std::move(myPeerId)
                                                      ), tick_(0) {
      CHECK_STRNE(myPeerId_.c_str(), "") << "myPeerId cannot be empty";
      LOG(INFO) << "VectorClock initialized";
    }

    Tick VectorClock::tick() {
        tick_++;
        return tick_;
    }

    void VectorClock::update(const Tick tick) {
        CHECK_LT(tick_, tick) << "new tick value should be strictly greater "
                                 "than current tick value";
        tick_ = tick;
    }

    Tick VectorClock::getTick() {
        return tick_;
    }

    ClockState VectorClock::getState() {
        return {{myPeerId_, tick_}};
    }
}