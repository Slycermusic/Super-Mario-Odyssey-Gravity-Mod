#pragma once

#include "JSystem/JGeometry.hpp"

namespace MR {

inline void makeMtxRotate(MtxPtr m, const TVec3f& r) {
    m->makeR(r);
}
inline void makeMtxTR(MtxPtr m, const TVec3f& t, const TVec3f& r) {
    m->makeRT(r, t);
}
inline void makeMtxTRS(MtxPtr m, const TVec3f& t, const TVec3f& r, const TVec3f& s) {
    m->makeSRT(s, r, t);
}
inline void preScaleMtx(MtxPtr m, const TVec3f& s) {
    m->scaleBases(s.x, s.y, s.z);
}

}
