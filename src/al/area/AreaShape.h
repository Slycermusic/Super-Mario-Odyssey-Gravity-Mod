#pragma once

#include "al/hio/HioNode.h"
#include "sead/math/seadMatrix.h"
#include "sead/math/seadVector.h"
#include "sead/math/seadBoundBox.h"


namespace al {
class AreaShape : public HioNode {
public:
    AreaShape(void);

    virtual bool isInVolume(const sead::Vector3f&) const = 0;
    virtual bool isInVolumeOffset(const sead::Vector3f&, f32) const = 0;
    virtual bool calcNearestEdgePoint(sead::Vector3f*, const sead::Vector3f&) const = 0;
    virtual bool checkArrowCollision(sead::Vector3f*, sead::Vector3f*, const sead::Vector3f&, const sead::Vector3f&) const = 0;
    virtual bool calcLocalBoundingBox(sead::BoundBox3f*) const = 0;

    sead::Vector3f getScale() const { return mScale; }

    void setBaseMtxPtr(const sead::Matrix34f* baseMtxPtr);
    void setScale(const sead::Vector3f& scale);

    bool calcLocalPos(sead::Vector3f* localPos, const sead::Vector3f& trans) const;
    bool calcWorldPos(sead::Vector3f* worldPos, const sead::Vector3f& trans) const;
    bool calcWorldDir(sead::Vector3f* worldDir, const sead::Vector3f& trans) const;
    void calcTrans(sead::Vector3f* trans) const;

    void *hioNodePtr; // 0x0
    sead::Matrix34f* mBaseMtx; // 0x8
    sead::Vector3f mScale; // = sead::Vector3f::ones; // 0x10

};

}