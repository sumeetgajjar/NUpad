//
// Created by sumeet on 11/18/20.
//

#ifndef NUPAD_CLOCK_H
#define NUPAD_CLOCK_H

#include <glog/logging.h>
#include <types.h>

#include <utility>

namespace nupad::clock {
    class VectorClock {
        const PeerId myPeerId_;
        Tick tick_;
    public:
        explicit VectorClock(PeerId myPeerId);

        Tick tick();

        void update(Tick tick);

        Tick getTick();

        ClockState getState();
    };
}

#endif //NUPAD_CLOCK_H
