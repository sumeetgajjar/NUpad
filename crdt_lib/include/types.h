//
// Created by sumeet on 11/22/20.
//

#ifndef NUPAD_TYPES_H
#define NUPAD_TYPES_H

#include <string>
#include <unordered_map>

namespace nupad {
    typedef std::string PeerId;

    namespace clock {
        typedef int Tick;
        typedef std::unordered_map<PeerId, Tick> ClockState;
    }
}

#endif //NUPAD_TYPES_H
