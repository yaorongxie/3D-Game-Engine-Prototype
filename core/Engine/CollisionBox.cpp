#include "stdafx.h"
#include "CollisionBox.h"
#include "RenderObj.h"
#include "jsonUtil.h"

CollisionBox::CollisionBox(RenderObj* obj, Physics* p) : Component(obj)
{
	mPhysics = p;
	mPhysics->AddCollisionBox(this);
}

CollisionBox::~CollisionBox()
{
	mPhysics->RemoveCollisionBox(this);
	mPhysics = nullptr;
}

void CollisionBox::LoadProperties(const rapidjson::Value& properties)
{
	GetVectorFromJSON(properties, "min", mBox.mMin);
	GetVectorFromJSON(properties, "max", mBox.mMax);
}

Physics::AABB CollisionBox::GetAABB() const
{
	// We don¡¯t support non-uniform scale, so just take the x component of the scale vector
	Matrix4 scaleMatrix = Matrix4::CreateScale(mRenderObj->GetObjectConstants().modelToWorldMatrix.GetScale().x);
	Matrix4 translationMatrix = Matrix4::CreateTranslation(mRenderObj->GetPosition());
	Matrix4 transformMatrix = scaleMatrix * translationMatrix;

	Vector4 scaledMax(mBox.mMax.x, mBox.mMax.y, mBox.mMax.z, 1.0f);
	scaledMax = Transform(scaledMax, transformMatrix);
	Vector4 scaledMin(mBox.mMin.x, mBox.mMin.y, mBox.mMin.z, 1.0f);
	scaledMin = Transform(scaledMin, transformMatrix);

	Physics::AABB scaledBox;
	scaledBox.mMax.x = scaledMax.x;
	scaledBox.mMax.y = scaledMax.y;
	scaledBox.mMax.z = scaledMax.z;
	scaledBox.mMin.x = scaledMin.x;
	scaledBox.mMin.y = scaledMin.y;
	scaledBox.mMin.z = scaledMin.z;

	return scaledBox;
}