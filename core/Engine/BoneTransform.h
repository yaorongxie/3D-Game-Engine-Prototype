#pragma once
#include "engineMath.h"

class BoneTransform
{
public:
	BoneTransform() {};
	BoneTransform(Quaternion& q, Vector3& v);
	Quaternion mRotation;
	Vector3 mTranslation;
	Matrix4 ToMatrix() const;	
	static BoneTransform Interpolate(const BoneTransform& a, const BoneTransform& b, float f);
};

