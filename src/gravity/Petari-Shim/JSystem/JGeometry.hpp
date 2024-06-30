#pragma once

#include <math/seadMatrix.h>
#include "JMath.hpp"

typedef float f32;
typedef int s32;

class TVec3f : public sead::Vector3f {
public:
    TVec3f() {}
    TVec3f(sead::Vector3f v) : sead::Vector3f(v) {}
    TVec3f(f32 x, f32 y, f32 z) : sead::Vector3f(x, y, z) {}
    TVec3f(s32 x, s32 y, s32 z) : sead::Vector3f(x, y, z) {}

    friend TVec3f operator-(const TVec3f& a1, const TVec3f& a2) { return TVec3f(sead::Vector3f(a1) - sead::Vector3f(a2)); }
    friend TVec3f operator+(const TVec3f& a1, const TVec3f& a2) { return TVec3f(sead::Vector3f(a1) + sead::Vector3f(a2)); }
    f32 normalize(const sead::Vector3f& other) {
        this->e = other.e;
    	return sead::Vector3f::normalize();
    }
    TVec3f translate(const sead::Vector3f& other) {
        return sead::Vector3f(*this) + other;
    }
    bool isZero() const {
        return squared() < 0.0000038147f;
    }
    void zero() { this->e = sead::Vector3f::zero.e; }
    f32 squared() const { return squaredLength(); }
    void setInline(const sead::Vector3f& a) { this->e = a.e; }
    void setInlinePS(const sead::Vector3f& a) { this->e = a.e; }
    void scaleInline(f32 scale) { this->e = (*this * scale).e; }
    void negateInline(const sead::Vector3f& other) { this->e = (-other).e; }
    void subInline(const sead::Vector3f& a1, const sead::Vector3f& a2) { this->e = (a1-a2).e; }
    void subInline(const sead::Vector3f& a1) { this->e = (sead::Vector3f(*this)-a1).e; }
    void addInline(const sead::Vector3f& a1) { this->e = (sead::Vector3f(*this)+a1).e; }
    const TVec3f* toVec() const { return this; }
    TVec3f* toVec() { return this; }
    const TVec3f* toCVec() const { return this; }
    TVec3f* toCVec() { return this; }
    const sead::Vector3f* toSeadVec() const { return this; }
    sead::Vector3f* toSeadVec() { return this; }

    inline void rejection(const TVec3f &rVec, const TVec3f &rNormal) {
        JMAVECScaleAdd(rNormal.toCVec(), rVec.toCVec(), toVec(), -rNormal.dot(rVec));
    }
    inline TVec3f multInline2(f32 scalar) const {
        TVec3f f(*this);
        f.scaleInline(scalar);
        return f;
    }
    inline TVec3f negateInline_2() const {
        TVec3f ret;
        ret.negateInline(*this);
        return ret;
    }
    inline TVec3f _madd(const TVec3f &v) const {
        TVec3f ret(*this);
        ret.addInline(v);
        return ret;
    }
    inline TVec3f madd(f32 scale, const TVec3f &v) const {
        return _madd(v * scale);
    }
    inline f32 squareDistancePS(const TVec3f &rVec1) const {
        TVec3f v = *this;
        v.subInline(rVec1);
        return v.squared();
    }
};
typedef TVec3f Vec;

#define VECMag(x) (x)->length()
#define VECNormalize(x, y) (y)->normalize(*(x))

class TRot3f : public sead::Matrix34f {
public:
    void identity() { makeIdentity(); }
    void mult(const TVec3f& a1, TVec3f& a2) const {
	a2.x = m[0][3] + a1.z*m[0][2] + a1.x*m[0][0] + a1.y*m[0][1];
	a2.y = m[1][3] + a1.z*m[1][2] + a1.x*m[1][0] + a1.y*m[1][1];
	a2.z = m[2][3] + a1.z*m[2][2] + a1.x*m[2][0] + a1.y*m[2][1];
    }
    inline f32 yy(f32 y) {
        return y * y;
    }
    void setRotate(const TVec3f &mLocalDirection, f32 fr1e) {
        TVec3f v;
        v.set(mLocalDirection);
        VECMag(v.toCVec());
        VECNormalize(v.toCVec(), v.toVec());
        f32 fr1ey = sin(fr1e), fr1ex = cos(fr1e);
        f32 x, y, z;
        y = v.y;
        x = v.x;
        z = v.z;
        m[0][0] = fr1ex + (1.0f - fr1ex) * yy(x);
        m[0][1] = (1.0f - fr1ex) * x * y - fr1ey * z;
        m[0][2] = (1.0f - fr1ex) * x * z + fr1ey * y;
        m[1][0] = (1.0f - fr1ex) * x * y + fr1ey * z;
        m[1][1] = fr1ex + (1.0f - fr1ex) * yy(y);
        m[1][2] = (1.0f - fr1ex) * y * z - fr1ey * x;
        m[2][0] = (1.0f - fr1ex) * x * z - fr1ey * y;
        m[2][1] = (1.0f - fr1ex) * y * z + fr1ey * x;
        m[2][2] = fr1ex + (1.0f - fr1ex) * yy(z);
    }
};

class TPos3f : public sead::Matrix34f {
public:
    void identity() { makeIdentity(); }
    void concat(const TPos3f& other) {
	this->a = (*this * other).a;
    }
    void concat(const TPos3f& other1, const TPos3f& other2) {
        this->a = (other1 * other2).a;
    }

    void mult(const TVec3f& a1, TVec3f& a2) const {
	a2.x = m[0][3] + a1.z*m[0][2] + a1.x*m[0][0] + a1.y*m[0][1];
	a2.y = m[1][3] + a1.z*m[1][2] + a1.x*m[1][0] + a1.y*m[1][1];
	a2.z = m[2][3] + a1.z*m[2][2] + a1.x*m[2][0] + a1.y*m[2][1];
    }

    void mult33(const TVec3f& src, TVec3f& dst) const {
	dst.x = src.z*m[0][2] + src.x*m[0][0] + src.y*m[0][1];
	dst.y = src.z*m[1][2] + src.x*m[1][0] + src.y*m[1][1];
	dst.z = src.z*m[2][2] + src.x*m[2][0] + src.y*m[2][1];
    }

    void setInline(const TPos3f& other) {
        this->a = other.a;
    }
    void getXDirInline(TVec3f& v) const {
        getBase(v, 0);
    }
    void getXDir(TVec3f& v) const {
        getBase(v, 0);
    }
    void getYDir(TVec3f& v) const {
        getBase(v, 1);
    }
    void getZDir(TVec3f& v) const {
        getBase(v, 2);
    }
    void getTrans(TVec3f& v) const {
        getTranslation(v);
    }
    void getTransInline(TVec3f& v) const {
        getTranslation(v);
    }
    void setTrans(const TVec3f& v) {
        setBase(3, v);
    }
};
typedef TPos3f *MtxPtr;


namespace JGeometry {
inline void negateInternal(const f32* src, f32* dst) {
    auto& srcVec = *reinterpret_cast<const sead::Vector3f**>(&src);
    auto& dstVec = *reinterpret_cast<sead::Vector3f**>(&dst);
    *dstVec = -(*srcVec);
}
}
// wow!
