//
// Created by sumeet on 11/18/20.
//

#ifndef NUPAD_CLOCK_H
#define NUPAD_CLOCK_H

#include <unordered_map>

namespace nupad {
    typedef std::string PeerId;

    namespace clock {
        typedef uint32_t Tick;
        typedef std::unordered_map<PeerId, Tick> ClockState;

        class VectorClock {
            static bool initialized_;
            static PeerId myPeerId_;
            static ClockState clockState_;
        public:
            static void init(PeerId myPeerId);

            static void tick(Tick tick = 1);

            static void update(const PeerId &peerId, Tick tick);

            static Tick getTick(const PeerId &peerId = myPeerId_);

            static ClockState getState();
        };
    }
}

#endif //NUPAD_CLOCK_H
