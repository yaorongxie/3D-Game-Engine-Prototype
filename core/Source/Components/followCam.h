#pragma once
#include "component.h"
#include "engineMath.h"

class Game;

class FollowCam : public Component
{
public:
    FollowCam(RenderObj* pObj, Game* pGame);

    void LoadProperties(const rapidjson::Value& properties) override;

    void Update(float deltaTime) override;

private:
    Vector3 mOffset;
    Game* mGame;
};