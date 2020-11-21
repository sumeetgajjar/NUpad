//
// Created by sumeet on 11/18/20.
//

#ifndef NUPAD_CRDT_LIST_H
#define NUPAD_CRDT_LIST_H

#include <list>
#include <glog/logging.h>

namespace nupad::crdt {

    /* All definitions are in-place to avoid linker error, this is a standard practice
     * used in STL.
     * ref: https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor
     */
    template<typename T>
    class CRDTList {
    public:
        CRDTList() {
            LOG(INFO) << "CRDTList of " << typeid(T).name() << " initialized";
        }

        std::list<T> getContents() {
            return std::list<T>();
        }
    };
}

#endif //NUPAD_CRDT_LIST_H
