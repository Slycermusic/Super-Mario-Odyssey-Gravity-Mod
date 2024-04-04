#pragma once

#include "al/area/AreaObj.h"
#include "Library/Placement/PlacementFunction.h"
#include "Library/LiveActor/ActorPoseKeeper.h"

class JMapInfoIter : public al::AreaInitInfo {
public:
    JMapInfoIter(const al::AreaInitInfo& a) {
        memcpy(this, &a, sizeof(al::AreaInitInfo));
    }
    bool getValue(const char* name, const char** dst) const {
        return tryGetStringArg(dst, *this, name);
    }
};

namespace MR {

inline bool getArgAndInit(const JMapInfoIter& info, f32* out, const char* name) {
    *out = -1.0f;
    return tryGetArg(out, info, name);
}
inline bool getArgAndInit(const JMapInfoIter& info, s32* out, const char* name) {
    *out = -1;
    return tryGetArg(out, info, name);
}

inline bool getJMapInfoTrans(const JMapInfoIter& info, TVec3f* out) {
    return tryGetTrans(out, info);   
}
inline bool getJMapInfoRotate(const JMapInfoIter& info, TVec3f* out) {
    return tryGetRotate(out, info);   
}
inline bool getJMapInfoScale(const JMapInfoIter& info, TVec3f* out) {
    return tryGetScale(out, info);   
}
inline bool getJMapInfoArg0WithInit(const JMapInfoIter& info, f32* out) {
    return getArgAndInit(info, out, "arg0");
}
inline bool getJMapInfoArg1WithInit(const JMapInfoIter& info, f32* out) {
    return getArgAndInit(info, out, "arg1");
}
inline bool getJMapInfoArg2WithInit(const JMapInfoIter& info, f32* out) {
    return getArgAndInit(info, out, "arg2");
}
inline bool getJMapInfoArg0WithInit(const JMapInfoIter& info, s32* out) {
    return getArgAndInit(info, out, "arg0");
}
inline bool getJMapInfoArg1WithInit(const JMapInfoIter& info, s32* out) {
    return getArgAndInit(info, out, "arg1");
}
inline bool getJMapInfoArg2WithInit(const JMapInfoIter& info, s32* out) {
    return getArgAndInit(info, out, "arg2");
}
inline bool getJMapInfoArg0NoInit(const JMapInfoIter& info, s32* out) {
    return tryGetArg(out, info, "arg0");
}

}

