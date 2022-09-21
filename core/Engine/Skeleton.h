#pragma once
#include "BoneTransform.h"

class AssetManager;

class Skeleton
{
public:
	struct Bone
	{
		Bone() {};

		BoneTransform mTransform;
		std::string mName;
		int mParentIndex;
	};

	size_t GetNumBones() const;
	const Bone& GetBone(size_t idx) const;
	const std::vector<Bone>& GetBones() const;
	const std::vector<Matrix4>& GetGlobalInvBindPoses() const;

	bool Load(const WCHAR* fileName);
	static Skeleton* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);

private:
	int mBoneNum;
	std::vector<Bone> mBones;
	std::vector<Matrix4> mInverseBindPoseMatrices;

	// Compute the global inverse bind pose for each bone
	void ComputeGlobalInvBindPose();
};

