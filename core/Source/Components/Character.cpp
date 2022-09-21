#include "stdafx.h"
#include "Character.h"
#include "game.h"
#include "Skeleton.h"
#include "jsonUtil.h"
#include "SkinnedObj.h"
#include "assetManager.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include "Profiler.h"
#include "ProcessSkinMatrices.h"
#include "JobManager.h"

Character::Character(Game* game, SkinnedObj* pObj)
{
	mGame = game;
	mSkinnedObj = pObj;
	mRenderObj = pObj;
}

Character::~Character()
{

}

void Character::LoadProperties(const rapidjson::Value& properties)
{
	std::wstring skeletonFileName;
	if (GetWStringFromJSON(properties, "skeleton", skeletonFileName) == false)
	{
		DbgAssert(false, "Fail to load skeleton name");
	}
	mSkeleton = Skeleton::StaticLoad(skeletonFileName.c_str(), mGame->GetAssetManager());
	mGlobalPoseMatrices.clear();
	mGlobalPoseMatrices.resize(mSkeleton->GetNumBones());
	// Get Inverse Bind Pose Matrices
	mInverseBindPoseMatrices = mSkeleton->GetGlobalInvBindPoses();

	const rapidjson::Value& animationArray = properties["animations"];
	std::string shortName;
	std::string fileName;
	for (unsigned int i = 0; i < animationArray.Size(); i++)
	{
		shortName = animationArray[i][0].GetString();
		fileName = animationArray[i][1].GetString();
		std::wstring wsFileName(fileName.begin(), fileName.end());
		mAnimations[shortName] = mGame->GetAssetManager()->LoadAnimation(wsFileName);
	}
}

bool Character::SetAnim(const std::string& animName)
{
	std::unordered_map<std::string, const Animation*>::iterator it = mAnimations.find(animName);
	if (it != mAnimations.end())
	{
		mCurrentAnimation = it->second;
		mAnimationTimer = 0.0f;
		return true;
	}

	return false;
}

void Character::UpdateAnim(float deltaTime)
{
	PROFILE_SCOPE(UpdateAnim);

	// Update timer
	mAnimationTimer += deltaTime;
	// Wrap around the animation timer when it exceeds the animation length
	if (mCurrentAnimation->GetLength() > 0.0f && mAnimationTimer > mCurrentAnimation->GetLength())
	{
		while (mAnimationTimer > mCurrentAnimation->GetLength())
		{
			mAnimationTimer -= mCurrentAnimation->GetLength();
		}
	}

	// Create a job to do the remaining tasks
	ProcessSkinMatrices* mJob = new ProcessSkinMatrices(this);
	JobManager::Get()->AddJob(mJob);
}

void Character::Update(float deltaTime)
{
	PROFILE_SCOPE(CharacterUpdate);

	if (mCurrentAnimation == nullptr)
	{
		if (SetAnim("run") == false)
		{
			DbgAssert(false, "Cannot load non-existing animation");
		}
	}

	UpdateAnim(deltaTime);
}