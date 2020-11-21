//
// Created by sumeet on 11/18/20.
//

#include "clock.h"

#include <glog/logging.h>

namespace nupad::clock {

    bool VectorClock::initialized = false;

    void VectorClock::init() {
        CHECK(!initialized) << "VectorClock already initialized";
        initialized = true;
        LOG(INFO) << "VectorClock initialized";
    }
}