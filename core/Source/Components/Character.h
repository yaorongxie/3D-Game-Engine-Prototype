#pragma once
#include "Component.h"
#include "engineMath.h"
#include "ProcessSkinMatrices.h"

class SkinnedObj;
class Skeleton;
class Animation;
class Game;

class Character : public Component
{
	friend ProcessSkinMatrices;

	public:
		Character(Game* game, SkinnedObj* pObj);
		~Character();
		void LoadProperties(const rapidjson::Value& properties) override;
		bool SetAnim(const std::string& animName);
		void UpdateAnim(float deltaTime);
		void Update(float deltaTime) override;

	protected:
		Game* mGame;
		SkinnedObj* mSkinnedObj;
		Skeleton* mSkeleton;
		const Animation* mCurrentAnimation;
		std::vector<Matrix4> mGlobalPoseMatrices;
		std::vector<Matrix4> mInverseBindPoseMatrices;
		std::unordered_map<std::string, const Animation*> mAnimations;
		float mAnimationTimer = 0.0f;
};

