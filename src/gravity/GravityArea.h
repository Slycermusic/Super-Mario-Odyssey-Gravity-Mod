#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/area/AreaObj.h"

class GravityArea : public al::AreaObj {
public:
    GravityArea(const char* name);
    ~GravityArea();
    void init(al::AreaInitInfo const&);
    void calcRotatedGravity(sead::Vector3f& result);
    bool isValidAngleDeg(const sead::Vector3f& distance);
    void calcInverseDistance(sead::Vector3f& result, const al::LiveActor* actor);
    void calcPointGravity(sead::Vector3f& result, const al::LiveActor* actor);
    void calcCubeGravity(sead::Vector3f& result, const al::LiveActor* actor);
    void calcDiskGravity(sead::Vector3f& result, const al::LiveActor* actor);
    void calcDiskTorusGravity(sead::Vector3f& result, const al::LiveActor* actor);
    void calcSegmentGravity(sead::Vector3f& result, const al::LiveActor* actor);
    void calcParallelGravity(sead::Vector3f& result, const al::LiveActor* actor);
    void calcConeGravity(sead::Vector3f& result, const al::LiveActor* actor);
    void calcCylinderGravity(sead::Vector3f& result, const al::LiveActor* actor);
    void calcRailGravity(sead::Vector3f& result, const al::LiveActor* actor);

    float mRadius = 0.0f;
    float mHeight = 0.0f;
    float mRadiusX = 0.0f;
    float mRadiusY = 0.0f;
    float mRadiusZ = 0.0f;
    float mValidAngleDeg = 0.0f;
    int mValidSurfaces = 0;
    int mEdgeType = 0;
    bool isValidX = true;
    bool isValidY = true;
    bool isValidZ = true;
    bool isDisableBottom = false;
    bool isDisableEdges = false;
    bool isInverted = false;
    bool isValidAngle = false;
    al::Rail* mRail = nullptr;
};
