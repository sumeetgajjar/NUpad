//
// Created by sumeet on 11/18/20.
//

#ifndef NUPAD_CLOCK_H
#define NUPAD_CLOCK_H

#include <types.h>

namespace nupad::clock {
    class VectorClock {
        static bool initialized_;
        static PeerId myPeerId_;
        static ClockState clockState_;
    public:
        static void init(PeerId myPeerId);

        static Tick tick(Tick tick = 1);

        static void update(const PeerId &peerId, Tick tick);

        static Tick getTick(const PeerId &peerId = myPeerId_);

        static ClockState getState();
    };
}

#endif //NUPAD_CLOCK_H
