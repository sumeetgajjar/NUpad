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
        bool initialized_ = false;
        PeerId myPeerId_;
        Tick tick_;
    public:
        explicit VectorClock(PeerId myPeerId) {
          myPeerId_ = std::move(myPeerId);
          CHECK_STRNE(myPeerId_.c_str(), "") << "myPeerId cannot be empty";
          tick_ = 0;
          initialized_ = true;
          LOG(INFO) << "VectorClock initialized";
        }

        Tick tick();

        void update(Tick tick);

        Tick getTick();

        ClockState getState();
    };
}

#endif //NUPAD_CLOCK_H
