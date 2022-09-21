#include "stdafx.h"
#include "ProcessSkinMatrices.h"
#include "Components\Character.h"
#include "JobManager.h"
#include "Profiler.h"
#include "Skeleton.h"
#include "SkinnedObj.h"
#include "Animation.h"

ProcessSkinMatrices::ProcessSkinMatrices(Character* pCharacter)
{
	mCharacter = pCharacter;
}

ProcessSkinMatrices::~ProcessSkinMatrices()
{
	mCharacter = nullptr;
}

void ProcessSkinMatrices::DoIt()
{
	PROFILE_SCOPE(ProcessSkinMatrices);

	// Global Pose Matrices at a given frame
	{
		PROFILE_SCOPE(GetGlobalPoseAtTime);

		mCharacter->mCurrentAnimation->GetGlobalPoseAtTime(mCharacter->mGlobalPoseMatrices, mCharacter->mSkeleton, mCharacter->mAnimationTimer);
	}


	{
		PROFILE_SCOPE(ComputeAndStoreSkinMatrices);

		for (size_t i = 0; i < mCharacter->mSkeleton->GetNumBones(); i++)
		{
			mCharacter->mSkinnedObj->mSkinConstants.skinMatrices[i] = mCharacter->mInverseBindPoseMatrices[i] * mCharacter->mGlobalPoseMatrices[i];
		}
	}

	// Decrease job count when finishing execution
	JobManager::Get()->JobCountDecrement();

	// Delete the job itself
	delete this;
}