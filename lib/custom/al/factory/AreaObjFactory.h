#pragma once

#include "Library/Factory/Factory.h"

namespace al {
class AreaObj;

    struct AreaFactoryEntry {
        const char* areaName;
        al::AreaObj* (*const createAreaObjFunction)(const char* name);
    };

    template <typename T>
    AreaObj* createAreaObjFunction(const char* name);

    typedef al::AreaObj *(*createArea)(const char *name);

    class AreaObjFactory : public al::Factory<createArea> {
        public:
            AreaObjFactory(const char* fName) : Factory(fName) {}

    };


};
