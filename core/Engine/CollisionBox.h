#pragma once
#include "Component.h"
#include "Physics.h"

class RenderObj;

class CollisionBox : public Component
{
	public:
		CollisionBox(RenderObj* obj, Physics* p);
		~CollisionBox();
		void LoadProperties(const rapidjson::Value& properties) override;
		Physics::AABB GetAABB() const;

	protected:
		Physics* mPhysics;
		Physics::AABB mBox;
};

