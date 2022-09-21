#include "stdafx.h"
#include "Component.h"
#include "RenderObj.h"

Component::Component()
{
	mRenderObj = nullptr;
}

Component::Component(RenderObj* pObj) 
{ 
	mRenderObj = pObj; 
}

Component::~Component()
{
	mRenderObj = nullptr;
}

void Component::LoadProperties(const rapidjson::Value& properties)
{

}

void Component::Update(float deltaTime)
{

}
