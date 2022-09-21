#pragma once
#include "Component.h"

class RenderObj;

class SimpleRotate : public Component
{
	public:
		SimpleRotate(RenderObj* pObj);
		~SimpleRotate();
		void LoadProperties(const rapidjson::Value& properties) override;
		void Update(float deltaTime) override;

	protected:
		float mSpeed;
		float mRotation;
};

