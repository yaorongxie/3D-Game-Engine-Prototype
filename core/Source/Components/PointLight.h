#pragma once
#include "Component.h"
#include "Lights.h"

class Game;

class PointLight :	public Component
{
public:
	PointLight(Game* game, RenderObj* pObj);
	~PointLight();
	void LoadProperties(const rapidjson::Value& properties) override;
	void Update(float deltaTime) override;

private:
	Game* mGame;
	Lights::PointLightData* mPointLightData;
};

