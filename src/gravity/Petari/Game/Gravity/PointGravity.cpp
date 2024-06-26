#include "Game/Gravity.hpp"
#include "Game/Util.hpp"

#include "logger/Logger.hpp"

PointGravity::PointGravity() :
	PlanetGravity(),
	mOrigPosition(0, 0, 0),
	mTranslation(0, 0, 0)
{

}

bool PointGravity::calcOwnGravityVector(TVec3f *pDest, f32 *pScalar, const TVec3f &rPosition) const {
	TVec3f direction;
	direction.subInline(rPosition, mTranslation);

	f32 scalar = VECMag((Vec*)&direction);

	Logger::log("xPosition: %f, yPosition: %f, zPosition: %f\n", rPosition.x, rPosition.y, rPosition.z);
	Logger::log("xTranslation: %f, yTranslation: %f, zTranslation: %f\n", mTranslation.x, mTranslation.y, mTranslation.z);


	// Epsilon-equals zero? If so, direction is the zero vector.
	if (MR::isNearZero(scalar, 0.01f)) {
		direction.z = 0.0f;
		direction.y = 0.0f;
		direction.x = 0.0f;
	}
	// Otherwise, it's a proper direction and it should be normalized.
	else {
		direction.scaleInline(1.0f / scalar);
	}

	// Out of range? If so, don't update result vector & scalar
	if (!isInRangeDistance(scalar)) {
		return false;
	}
	// Update result vector & scalar if applicable
	else {
		if (pDest) {
			pDest->setInlinePS(direction);
		}
		if (pScalar) {
			*pScalar = scalar;
		}

		return true;
	}
}

void PointGravity::updateMtx(const TPos3f &rMtx) {
	// Converts relative zone-coordinates into worldspace
	// mTranslation = rMtx * mOrigPosition
	rMtx.mult(mOrigPosition, mTranslation);

	Logger::log("rMtxX: %f, rMtxY: %f, rMtxZ: %f,", rMtx.m[0][3], rMtx.m[1][3], rMtx.m[2][3]);
	Logger::log("xOrigPosition: %f, yOrigPosition: %f, zOrigPosition: %f\n", mOrigPosition.x, mOrigPosition.y, mOrigPosition.z);

}
