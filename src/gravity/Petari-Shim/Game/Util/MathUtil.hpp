#pragma once

#include "Game/Util/MtxUtil.hpp"

#include "logger/Logger.hpp"

namespace MR {

inline void separateScalarAndDirection(f32* scalar, TVec3f* dir, const TVec3f& in) {
    dir->e = in.e;
    *scalar = dir->toSeadVec()->normalize();
}

inline void normalizeOrZero(TVec3f* vec) {
    if(vec->squared() < 0.00001f) {
        vec->e = sead::Vector3f::zero.e;
    }
    vec->toSeadVec()->normalize();
}

inline bool isNearZero(f32 val, f32 epsilon) {
    return val > -epsilon && val < epsilon;
}

inline bool isNearZero(const TVec3f &rVec, f32 a2) {
    f32 v2 = rVec.x;
    if ( v2 > a2 )
        return 0;

    f32 v4 = -a2;
    if ( v2 < -a2 )
        return 0;

    f32 v5 = rVec.y;
    if ( v5 > a2 )
        return 0;

    if ( v5 < v4 )
        return 0;

    f32 v6 = rVec.z;

    if ( v6 > a2 )
        return 0;

    return !(rVec.z < -a2);
}

inline void normalize(const TVec3f & in, TVec3f * out) {
    out->e = in.e;
    out->toSeadVec()->normalize();
}

inline void normalize(TVec3f * vec) {
    vec->toSeadVec()->normalize();
}

inline f32 getMaxElement(const TVec3f& vec) {
    if(vec.x >= vec.y && vec.x >= vec.z) return vec.x;
    if(vec.y >= vec.x && vec.y >= vec.z) return vec.y;
    if(vec.z >= vec.x && vec.z >= vec.y) return vec.z;
    // something went wrong
    return -1;
}

inline f32 calcPerpendicFootToLineInside(TVec3f *pOut, const TVec3f &rPoint, const TVec3f &rTip, const TVec3f &rTail) {
    TVec3f line = rTip - rTail;
    line.normalize(line);
    TVec3f relativePoint = rPoint - rTail;
    f32 pointPos = line.dot(relativePoint) / (rTip-rTail).length();
    pointPos = sead::Mathf::clamp(pointPos, 0.0f, 1.0f);
    TVec3f outputPoint = pointPos * line;
    outputPoint += rTail;
    return pointPos;
}

inline void makeAxisVerticalZX(TVec3f * out, const TVec3f & axis) {
    TVec3f ortho1, ortho2;
    ortho1.rejection(axis, TVec3f(1, 0, 0));
    ortho2.rejection(axis, TVec3f(0, 0, 1));
    if(ortho1.squared() < 0.00001f) {
        *out = ortho2;
    } else {
        *out = ortho1;
    }
    normalize(out);
}

}

