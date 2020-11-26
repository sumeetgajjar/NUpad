//
// Created by sumeet on 11/18/20.
//

#include "clock.h"

namespace nupad::clock {

    Tick VectorClock::tick() {
        CHECK(initialized_) << "VectorClock not initialized";
        tick_++;
        return tick_;
    }

    void VectorClock::update(const Tick tick) {
        CHECK(initialized_) << "VectorClock not initialized";
        CHECK_LT(tick_, tick) << "new tick value should be strictly greater "
                                 "than current tick value";
        tick_ = tick;
    }

    Tick VectorClock::getTick() {
        CHECK(initialized_) << "VectorClock not initialized";
        return tick_;
    }

    ClockState VectorClock::getState() {
        CHECK(initialized_) << "VectorClock not initialized";
        return {{myPeerId_, tick_}};
    }
}