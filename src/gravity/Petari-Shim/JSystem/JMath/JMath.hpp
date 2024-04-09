#pragma once

#include <math/seadMathCalcCommon.h>

#define JMAVECScaleAdd(a, b, out, scale) *(out) = *(a) * (scale) + *(b)
#define __fabsf fabsf

namespace MR {
inline f32 sqrt(f32 x) { return sead::Mathf::sqrt(x); }
}
