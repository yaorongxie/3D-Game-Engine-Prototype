#pragma once
#include "BoneTransform.h"

class AssetManager;
class Skeleton;

class Animation
{
public:
	uint32_t GetNumBones() const;
	uint32_t GetNumFrames() const;
	float GetLength() const;
	bool Load(const WCHAR* fileName);
	static Animation* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);
	void GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const;

private:
	uint32_t numBones;
	uint32_t numFrames;
	float mLength = 0.0f;
	// mAnimationData[i][j] -> bone #i and key frame #j
	std::vector<std::vector<BoneTransform>> mAnimationData;
	std::vector<float> mFrameTime;
};

