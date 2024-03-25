#include <math/seadQuat.h>
#include <math/seadVector.h>

#include "al/util.hpp"
#include "al/util/VectorUtil.h"
#include "lib.hpp"
#include "logger/Logger.hpp"

extern al::LiveActor* currentActor;

sead::Vector3f someSubOperation(const sead::Quatf& a1, const sead::Vector3f& a2) {
    return a1.vector().cross(a2) + a2*a1.scalar();
}

sead::Vector3f someOperation(const sead::Quatf& a1, const sead::Vector3f& a2) {
    sead::Vector3f x1 = someSubOperation(a1, a2);
    float x2 = a1.vector().dot(-a2);
    return someSubOperation(a1, x1) - x2*a1.vector();
}

bool customTurnQuatFrontToDirDegreeH(sead::Quatf* a1, const sead::Vector3f& a2, float a3) {
    sead::Vector3f myUp = -al::getGravity(currentActor);

    sead::Vector3f x0 = a2;
    //x0.y = 0.0f;
    al::verticalizeVec(&x0, myUp, x0);
    if(x0.squaredLength() < 0.000001f) {
        x0 = {0.0f, 0.0f, 0.0f};
        return true;
    }
    
    sead::Vector3f front, up, side, gravity;
    al::calcQuatFront(&front, *a1);
    al::calcQuatUp(&up, *a1);
    al::calcQuatSide(&side, *a1);
    al::calcQuatGravity(&gravity, *a1);

    x0.normalize();
    sead::Vector3f x3 = someOperation(*a1, sead::Vector3f::ez);

    if(!(
        x3.dot(a2) >= 0.0f ||
        sead::Mathf::abs(x3.cross(a2).x) > 0.01f ||
        sead::Mathf::abs(x3.cross(a2).y) > 0.01f ||
        sead::Mathf::abs(x3.cross(a2).z) > 0.01f
    )) {
        sead::Vector3f x6 = someOperation(*a1, sead::Vector3f::ex);
        x0 = a2 + x6 * 0.01f;
    }
    bool check = al::turnQuat(a1, *a1, x3, x0, sead::Mathf::deg2rad(a3));
    // turnQuatYDirRate(a1, *a1, {0.0f, 1.0f, 0.0f}, 0.2f);
    al::turnQuatYDirRate(a1, *a1, myUp, 0.2f);
    return check;
}

HOOK_DEFINE_TRAMPOLINE(TurnQuatFrontToDirDegreeHFix) {
    static bool Callback(sead::Quatf* a1, const sead::Vector3f& a2, float a3) {
        return customTurnQuatFrontToDirDegreeH(a1, a2, a3);
    }
};


void gravityActorPatches() {
    TurnQuatFrontToDirDegreeHFix::InstallAtSymbol("_ZN2al25turnQuatFrontToDirDegreeHEPN4sead4QuatIfEERKNS0_7Vector3IfEEf");
}
