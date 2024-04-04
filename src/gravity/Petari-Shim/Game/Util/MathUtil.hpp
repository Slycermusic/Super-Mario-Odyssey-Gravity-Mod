#pragma once

#include "Game/Util/MtxUtil.hpp"

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

inline f32 getMaxElement(const TVec3f& vec) {
    if(vec.x >= vec.y && vec.x >= vec.z) return vec.x;
    if(vec.y >= vec.x && vec.y >= vec.z) return vec.y;
    if(vec.z >= vec.x && vec.z >= vec.y) return vec.z;
    // something went wrong
    return -1;
}
}

