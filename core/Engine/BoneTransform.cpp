#include "stdafx.h"
#include "BoneTransform.h"

BoneTransform::BoneTransform(Quaternion& q, Vector3& v)
{
	mRotation = q;
	mTranslation = v;
}

Matrix4 BoneTransform::ToMatrix() const
{
	return Matrix4::CreateFromQuaternion(mRotation) * Matrix4::CreateTranslation(mTranslation);
}

BoneTransform BoneTransform::Interpolate(const BoneTransform& a, const BoneTransform& b, float f)
{
	BoneTransform temp;
	temp.mRotation = Slerp(a.mRotation, b.mRotation, f);
	temp.mTranslation = Lerp(a.mTranslation, b.mTranslation, f);
	return temp;
}