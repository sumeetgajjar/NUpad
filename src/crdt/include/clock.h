//
// Created by sumeet on 11/18/20.
//

#ifndef NUPAD_CLOCK_H
#define NUPAD_CLOCK_H

namespace nupad::clock {
    class VectorClock {
        static bool initialized;
    public:
        static void init();
    };
}

#endif //NUPAD_CLOCK_H
