#pragma once

#include "al/area/AreaObj.h"

#include "Game/Gravity/GravityCreator.hpp"

class GravityArea : public al::AreaObj {
public:
    GravityArea(const char* name);
    ~GravityArea();
    void init(const al::AreaInitInfo&);

public:
    GravityCreator *mCreator;
};
