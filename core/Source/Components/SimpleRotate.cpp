#include "stdafx.h"
#include "SimpleRotate.h"
#include "game.h"
#include "RenderObj.h"
#include "jsonUtil.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"

SimpleRotate::SimpleRotate(RenderObj* pObj) : Component(pObj)
{
	mSpeed = 0.0f;
	mRotation = 0.0f;
}

SimpleRotate::~SimpleRotate()
{

}

void SimpleRotate::LoadProperties(const rapidjson::Value& properties)
{
	if (GetFloatFromJSON(properties, "speed", mSpeed) == false)
	{
		DbgAssert(false, "Fail to load light color for point light component");
	}
}

void SimpleRotate::Update(float deltaTime)
{
	// Update rotation
	mRotation += (mSpeed * deltaTime);
	Matrix4 rotation = Matrix4::CreateRotationZ(mRotation);

	// Extract scale and translation, which should remain unchanged
	Matrix4 prevMat = mRenderObj->GetObjectConstants().modelToWorldMatrix;
	Matrix4 scale = Matrix4::CreateScale(prevMat.GetScale());
	Matrix4 translation = Matrix4::CreateTranslation(prevMat.GetTranslation());
	
	// Compute and update model to world matrix
	PerObjectConstants objConstant;
	objConstant.modelToWorldMatrix = scale * rotation * translation;
	mRenderObj->SetObjectConstants(objConstant);
}