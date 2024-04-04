#include "gravity/GravityArea.h"
#include "Library/Base/String.h"
#include "lib.hpp"

GravityArea::GravityArea(const char* name) : AreaObj(name) {
    if (al::isEqualString(name, "CubeGravityArea")) {
        mCreator = new CubeGravityCreator();
    } else if (al::isEqualString(name, "DiskGravityArea")) {
        mCreator = new DiskGravityCreator();
    } else if (al::isEqualString(name, "DiskTorusGravityArea")) {
        mCreator = new DiskTorusGravityCreator();
    } else if (al::isEqualString(name, "ConeGravityArea")) {
        mCreator = new ConeGravityCreator();
    } else if (al::isEqualString(name, "PlaneGravityArea")) {
        mCreator = new PlaneGravityCreator();
    } else if (al::isEqualString(name, "PlaneInBoxGravityArea")) {
        mCreator = new PlaneInBoxGravityCreator();
    } else if (al::isEqualString(name, "PlaneInCylinderGravityArea")) {
        mCreator = new PlaneInCylinderGravityCreator();
    } else if (al::isEqualString(name, "PointGravityArea")) {
        mCreator = new PointGravityCreator();
    } else if (al::isEqualString(name, "SegmentGravityArea")) {
        mCreator = new SegmentGravityCreator();
    } else if (al::isEqualString(name, "WireGravityArea")) {
        mCreator = new WireGravityCreator();
    } else {
        EXL_ABORT(0, "Unrecognized Area Name: %s", name);
    }
}

GravityArea::~GravityArea() { delete mCreator; }

void GravityArea::init(const al::AreaInitInfo& info) {
    al::AreaObj::init(info);
    mCreator->createFromJMap(JMapInfoIter(info));
}
