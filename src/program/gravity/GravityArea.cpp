#include "gravity/GravityArea.h"
#include "al/Rail.h"
#include "al/placement/PlacementFunction.h"
#include "al/util/LiveActorUtil.h"
#include "al/util/MathUtil.h"
#include "al/util/StringUtil.h"
#include "logger/Logger.hpp"

GravityArea::GravityArea(const char* name) : AreaObj(name) {}

GravityArea::~GravityArea() {
    if (mRail) {
        delete mRail;
        mRail = nullptr;
    }
}

void GravityArea::init(al::AreaInitInfo const& info) {
    al::AreaObj::init(info);
    al::tryGetAreaObjArg(&isInverted, this, "IsInverted");
    al::tryGetAreaObjArg(&mRadiusX, this, "RadiusX");
    al::tryGetAreaObjArg(&mRadiusY, this, "RadiusY");
    al::tryGetAreaObjArg(&mRadiusZ, this, "RadiusZ");
    al::tryGetAreaObjArg(&isValidX, this, "IsValidXAxis");
    al::tryGetAreaObjArg(&isValidY, this, "IsValidYAxis");
    al::tryGetAreaObjArg(&isValidZ, this, "IsValidZAxis");
    al::tryGetAreaObjArg(&mRadius, this, "Radius");
    al::tryGetAreaObjArg(&mHeight, this, "Height");
    al::tryGetAreaObjArg(&mValidAngleDeg, this, "ValidAngleDeg");
    al::tryGetAreaObjArg(&isDisableBottom, this, "IsDisableBottom");
    al::tryGetAreaObjArg(&isDisableEdges, this, "IsDisableEdges");
    al::tryGetAreaObjArg(&mEdgeType, this, "EdgeType");
    al::tryGetAreaObjArg(&mValidSurfaces, this, "ValidSurfaces");

    al::PlacementInfo pInfo;
    if(al::tryGetLinksInfo(&pInfo, info, "Rail")) {
        mRail = new al::Rail();
        mRail->init(pInfo);
    }
    
}

void GravityArea::calcRotatedGravity(sead::Vector3f& result) {
    if (isInverted) result = -result;
    sead::Matrix33f mtx = sead::Matrix33f::zero;
    sead::Matrix33CalcCommon<float>::copy(mtx, mAreaTR);
    sead::Vector3CalcCommon<float>::mul(result, mtx, result);
}

bool GravityArea::isValidAngleDeg(const sead::Vector3f& distance) {
    if (mValidAngleDeg  <= 0) return true;
    sead::Vector3f axis = sead::Vector3f::ex;
    sead::Vector3f dist = {-distance.x, 0, -distance.z};
    float angle = al::calcAngleDegree(dist, axis);
    if (al::sign(distance.z) == -1)
        angle = 360.0f - angle;
    if (angle <= mValidAngleDeg)
        return true;
    else
        return false;
}

void GravityArea::calcInverseDistance(sead::Vector3f& result, const al::LiveActor* actor) {
    sead::Matrix33f inverse = sead::Matrix33f::zero;
    sead::Vector3f areaTrans = sead::Vector3f::zero;
    sead::Vector3f actorTrans = al::getTrans(actor);
    sead::Matrix33CalcCommon<float>::copy(inverse, mAreaTR);
    sead::Matrix33CalcCommon<float>::inverse(inverse, inverse);
    sead::Matrix34CalcCommon<float>::getTranslation(areaTrans, mAreaTR);
    result = areaTrans - actorTrans;
    sead::Vector3CalcCommon<float>::mul(result, inverse, result);
    isValidAngle = isValidAngleDeg(result);
}

void GravityArea::calcPointGravity(sead::Vector3f& result, const al::LiveActor* actor) {
    sead::Vector3f areaTrans = sead::Vector3f::zero;
    sead::Matrix34CalcCommon<float>::getTranslation(areaTrans, mAreaTR);
    sead::Vector3f actorTrans = al::getTrans(actor);
    result = areaTrans - actorTrans;
    if (isInverted) result = -result;
}

void GravityArea::calcCubeGravity(sead::Vector3f& result, const al::LiveActor* actor) {
    calcInverseDistance(result, actor);
    if (std::abs(result.x) < mRadiusX && std::abs(result.y) < mRadiusY && std::abs(result.z) < mRadiusZ) {
        result = -result;
    } else {
        sead::Vector3f closestPointInBox = {
            al::clamp(result.x,-mRadiusX,mRadiusX),
            al::clamp(result.y, -mRadiusY, mRadiusY),
            al::clamp(result.z, -mRadiusZ, mRadiusZ)
        };
        result = result - closestPointInBox;
    }
    if (!isValidX)
        result -= result.x*sead::Vector3f::ex;
    if (!isValidY)
        result -= result.y*sead::Vector3f::ey;
    if (!isValidZ)
        result -= result.z*sead::Vector3f::ez;
    calcRotatedGravity(result);
}

void GravityArea::calcDiskGravity(sead::Vector3f& result, const al::LiveActor* actor) {
    calcInverseDistance(result, actor);
    if (isValidAngle) {
        if (sqrtf(result.x * result.x + result.z * result.z) < mRadius) {
            result = {0, result.y, 0};
        } else if (!isDisableEdges) {
            float gravityPoint = sqrtf(mRadius * mRadius / (result.x * result.x + result.z * result.z));
            result = {result.x * (1 - gravityPoint), result.y, result.z * (1 - gravityPoint)};
        }
    } else {
        result = sead::Vector3f::zero;
    }
    calcRotatedGravity(result);
}

void GravityArea::calcDiskTorusGravity(sead::Vector3f& result, const al::LiveActor* actor) {
    calcInverseDistance(result, actor);
    float gravityPoint = sqrtf(mRadius*mRadius/(result.x*result.x + result.z*result.z));
    result = {result.x*(1-gravityPoint), result.y, result.z*(1-gravityPoint)};
    calcRotatedGravity(result);
}

void GravityArea::calcParallelGravity(sead::Vector3f& result, const al::LiveActor* actor) {
    calcInverseDistance(result, actor);
    if (isValidAngle) {
        result = -sead::Vector3f::ey;
    } else {
        result = sead::Vector3f::zero;
    }
    calcRotatedGravity(result);
}

void GravityArea::calcSegmentGravity(sead::Vector3f& result, const al::LiveActor* actor) {
    calcInverseDistance(result, actor);
    if (isValidAngle) {
        if (result.y > mRadius) {
            result = {result.x, result.y + mRadius, result.z};
        } else if (result.y < -mRadius) {
            result = {result.x, result.y - mRadius, result.z};
        } else {
            result = {result.x, 0, result.z};
        }
    } else {
        result = sead::Vector3f::zero;
    }
    calcRotatedGravity(result);
}

void GravityArea::calcConeGravity(sead::Vector3f& result, const al::LiveActor* actor) {
    calcInverseDistance(result, actor);
    float distXZ = sqrtf(result.x*result.x + result.z*result.z);
    if (result.y > 0) {
        result = sead::Vector3f::zero;
    } else if (std::abs(result.y) > mHeight && distXZ <= std::abs(mRadius/mHeight*(mHeight + result.y))) {
        result = {result.x, result.y + mHeight, result.z};
    } else {
        result = {mHeight*result.x/distXZ, -mRadius, mHeight*result.z/distXZ};
    }
    calcRotatedGravity(result);
}

void GravityArea::calcRailGravity(sead::Vector3f& result, const al::LiveActor* actor) {
    if(!mRail) {
        result = sead::Vector3f::zero;
        return;
    }
    sead::Vector3f actorTrans = al::getTrans(actor);
    sead::Vector3f railPos;
    mRail->calcNearestRailPos(&railPos, actorTrans, 7.5f);
    result = railPos - actorTrans;
    Logger::log("Ended with %.02f length > %.02f radius\n", result.length(), mRadius);
    if(result.length() > mRadius) {
        result = sead::Vector3f::zero;
    }
}
