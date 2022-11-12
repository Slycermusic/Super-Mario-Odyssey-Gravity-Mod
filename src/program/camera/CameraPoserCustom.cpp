#include "cameras/CameraPoserCustom.h"
#include <math.h>
#include "al/camera/CameraPoser.h"
#include "al/camera/CameraVerticalAbsorber2DGalaxy.h"
#include "al/camera/alCameraPoserFunction.h"
#include "al/util.hpp"
#include "al/util/MathUtil.h"
#include "al/util/VectorUtil.h"
#include "Logger/logger.hpp"
#include "sead/gfx/seadCamera.h"
#include "sead/math/seadVector.h"
#include "sead/math/seadVectorCalcCommon.h"

namespace cc {

    CameraPoserCustom::CameraPoserCustom(const char* poserName) : CameraPoser(poserName) {

        this->initOrthoProjectionParam();
    }

    void CameraPoserCustom::init(void) {
        alCameraPoserFunction::initSnapShotCameraCtrlZoomRollMove(
            this);  // this makes the snapshot camera have the abilities of the normal snapshot cam, but
                   // is locked rotationally
        alCameraPoserFunction::initCameraVerticalAbsorber(this);
        alCameraPoserFunction::initCameraAngleCtrl(this);
        mGalaxyAbsorber = new al::CameraVerticalAbsorber2DGalaxy();
        alCameraPoserFunction::offVerticalAbsorb(this);
        alCameraPoserFunction::initCameraArrowCollider(this);
    }

    void CameraPoserCustom::loadParam(al::ByamlIter const& paramIter) {
        al::tryGetByamlF32(&mOffsetY, paramIter, "OffsetY");
        al::tryGetByamlF32(&mDist, paramIter, "Distance");
        al::tryGetByamlF32(&mDirSnapSpeed, paramIter, "GravitySnapSpeed");
        al::tryGetByamlBool(&mIsResetAngleIfSwitchTarget, paramIter, "IsResetAngleIfSwitchTarget");
    }

    void normalize2(sead::Vector3f &v, float inv) {
        float len = sead::Vector3CalcCommon<float>::length(v);

        if (len > 0)
        {
            float inv_len = inv / len;
            v.x *= inv_len;
            v.y *= inv_len;
            v.z *= inv_len;
        }
    }

    void CameraPoserCustom::start(al::CameraStartInfo const&) {

        sead::Vector3f faceDir;
        sead::Vector3f targetFront;

        if (alCameraPoserFunction::isSceneCameraFirstCalc(this)) {
            alCameraPoserFunction::calcTargetTrans(&mTargetTrans, this);
            mTargetTrans.y += mOffsetY;
            targetFront = sead::Vector3f(0, 0, 0);
            alCameraPoserFunction::calcTargetFront(&targetFront, this);

            faceDir = mTargetTrans - (mDist * targetFront);

        } else {
            sead::LookAtCamera* curLookCam = alCameraPoserFunction::getLookAtCamera(this);

            sead::Vector3f curPos = curLookCam->getPos();

            sead::Vector3f curAt = curLookCam->getAt();

            targetFront = sead::Vector3f(curPos.x - curAt.x, 0.0, curPos.z - curAt.z);

            al::tryNormalizeOrDirZ(&targetFront);

            faceDir = (mDist * targetFront) + mTargetTrans;
        }

        mGalaxyAbsorber->start(this);

        mPosition = faceDir;
    }

    void CameraPoserCustom::update(void) {
        sead::Vector3f targetDir;

        if (alCameraPoserFunction::isChangeSubTarget(this) && mIsResetAngleIfSwitchTarget) {
            alCameraPoserFunction::calcTargetTrans(&mTargetTrans, this);
            mTargetTrans.y += mOffsetY;
            targetDir = sead::Vector3f(0, 0, 0);
            alCameraPoserFunction::calcTargetFront(&targetDir, this);

            mPosition = mTargetTrans - (mDist * targetDir);
        }

        sead::Vector3f targetGrav = sead::Vector3f::ey;

        mIsPrevTargetCollideGround = mIsTargetCollideGround;
        mIsTargetCollideGround = alCameraPoserFunction::isTargetCollideGround(this);

        alCameraPoserFunction::calcTargetGravity(&targetGrav, this);

        al::lerpVec(&mCameraUp, mCameraUp, -targetGrav, mDirSnapSpeed);

        mPrevTargetTrans = mTargetTrans;

        alCameraPoserFunction::calcTargetTrans(&mTargetTrans, this);
        mTargetTrans += mCameraUp * mOffsetY;

        if (!mIsTargetCollideGround) {
            sead::Vector3f deltaPos = mTargetTrans - mPrevTargetTrans;
            float verticalPos = -(deltaPos.x * targetGrav.x + deltaPos.y * targetGrav.y + deltaPos.z * targetGrav.z);
            mTargetTrans += verticalPos * targetGrav;

        } else if (!mIsPrevTargetCollideGround) {
            mGroundTimer = 50;
        }
        if (mGroundTimer != 0) {
            al::lerpVec(&mTargetTrans, mPrevTargetTrans, mTargetTrans, mGroundSnapSpeed);
            mGroundTimer--;
        }

        targetDir = mPosition - mTargetTrans;



        al::tryNormalizeOrDirZ(&targetDir);

        sead::Vector2f playerInput(0, 0);
        alCameraPoserFunction::calcCameraRotateStick(&playerInput, this);

        float stickSpeed = alCameraPoserFunction::getStickSensitivityScale(this) *
                           alCameraPoserFunction::getStickSensitivityLevel(this);

        sead::Vector3f preLook;
        alCameraPoserFunction::calcPreLookDir(&preLook, this);

        // Horizontal Rotation
        al::rotateVectorDegree(&targetDir, targetDir, mCameraUp, playerInput.x * -stickSpeed);

        sead::Vector3f rotatedVec = targetDir;

        sead::Vector3f rightAxis;
        rightAxis.setCross(targetDir, mCameraUp);

        // Vertical Rotation
        al::rotateVectorDegree(&rotatedVec, rotatedVec, rightAxis, playerInput.y * -stickSpeed);

        // mAngleH = al::calcAngleOnPlaneDegree(rotatedVec, mPrevTargetDir, mCameraUp);
        // mAngleV = al::calcAngleOnPlaneDegree(rotatedVec, mPrevTargetDir, rightAxis);

        // mGalaxyAbsorber->applyLimit(&rotatedVec);

        mPosition = mTargetTrans + (rotatedVec * mDist);

        mPrevTargetDir = rotatedVec;

        mGalaxyAbsorber->update(this);


    }

    void CameraPoserCustom::movement() {
        al::CameraPoser::movement();
    }

}