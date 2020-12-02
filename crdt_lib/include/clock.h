//
// Created by sumeet on 11/18/20.
//

#ifndef NUPAD_CLOCK_H
#define NUPAD_CLOCK_H

#include <types.h>

namespace nupad::clock {
    class VectorClock {
        const PeerId myPeerId_;
        ClockState clockState_{/*initial_capacity*/ 10};
    public:
        explicit VectorClock(PeerId myPeerId);

        VectorClock(const VectorClock &other) = default;

        Tick tick(Tick tick = 1);

        bool hasTick(const PeerId &peerId) const;

        void update(const PeerId &otherPeerId, Tick tick);

        Tick getTick(const PeerId &peerId) const;

        Tick getMyTick() const;

        ClockState getState() const;
    };
}

#endif //NUPAD_CLOCK_H
