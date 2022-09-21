#include "stdafx.h"
#include "PointLight.h"
#include "game.h"
#include "jsonUtil.h"
#include "RenderObj.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include "Profiler.h"

PointLight::PointLight(Game* game, RenderObj* pObj)
{
	mGame = game;
	mRenderObj = pObj;
	mPointLightData = mGame->AllocateLight();
}

PointLight::~PointLight()
{
	mGame->FreeLight(mPointLightData);
}

void PointLight::LoadProperties(const rapidjson::Value& properties)
{
	if (GetVectorFromJSON(properties, "lightColor", mPointLightData->lightColor) == false)
	{
		DbgAssert(false, "Fail to load light color for point light component");
	}

	if (GetFloatFromJSON(properties, "innerRadius", mPointLightData->innerRadius) == false)
	{
		DbgAssert(false, "Fail to load inner radius for point light component");
	}
	
	if (GetFloatFromJSON(properties, "outerRadius", mPointLightData->outerRadius) == false)
	{
		DbgAssert(false, "Fail to load outer radius for point light component");
	}
}

void PointLight::Update(float deltaTime)
{
	PROFILE_SCOPE(PointlightUpdate);

	mPointLightData->position = mRenderObj->GetPosition();
}