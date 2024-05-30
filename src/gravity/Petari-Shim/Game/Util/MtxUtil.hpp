#pragma once

#include "JSystem/JGeometry.hpp"

namespace MR {

inline void makeMtxRotate(MtxPtr m, const TVec3f& r) {
    m->makeR({sead::Mathf::deg2rad(r.x), sead::Mathf::deg2rad(r.y), sead::Mathf::deg2rad(r.z)});
}
inline void makeMtxTR(MtxPtr m, const TVec3f& t, const TVec3f& r) {
    m->makeRT({sead::Mathf::deg2rad(r.x), sead::Mathf::deg2rad(r.y), sead::Mathf::deg2rad(r.z)}, t);
}
inline void makeMtxTRS(MtxPtr m, const TVec3f& t, const TVec3f& r, const TVec3f& s) {
    m->makeSRT(s, {sead::Mathf::deg2rad(r.x), sead::Mathf::deg2rad(r.y), sead::Mathf::deg2rad(r.z)}, t);
}
inline void preScaleMtx(MtxPtr m, const TVec3f& s) {
    m->scaleBases(s.x, s.y, s.z);
}

}
