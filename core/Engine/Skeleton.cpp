#include "stdafx.h"
#include "Skeleton.h"
#include "assetManager.h"
#include "jsonUtil.h"
#include "stringUtil.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>
#include "Profiler.h"

void Skeleton::ComputeGlobalInvBindPose()
{
	PROFILE_SCOPE(ComputeGlobalInvBindPose);

	// Calculate the bind pose matrix for each bone
	Matrix4 bindPoseMatrix = mBones[0].mTransform.ToMatrix();
	int parentIndex = mBones[0].mParentIndex;
	mInverseBindPoseMatrices.push_back(bindPoseMatrix);

	for (unsigned int i = 1; i < mBones.size(); i++)
	{
		bindPoseMatrix = mBones[i].mTransform.ToMatrix();
		parentIndex = mBones[i].mParentIndex;

		// Multiply the Bind Pose of the child bone by its parent bone 
		// and its parent bone all the way up the chain
		bindPoseMatrix *= mInverseBindPoseMatrices[parentIndex];

		// Store the bind pose matrix and inverse it later
		mInverseBindPoseMatrices.push_back(bindPoseMatrix);
	}

	// Inverse the bind pose matrices
	for (unsigned int i = 0; i < mInverseBindPoseMatrices.size(); i++)
	{
		mInverseBindPoseMatrices[i].Invert();
	}
}

bool Skeleton::Load(const WCHAR* fileName)
{
	// Open file
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return false;
	}

	// Create file stream
	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	// Check whether open file successfully
	if (!doc.IsObject())
	{
		DbgAssert(false, "Unable to open Skeleton file");
		return false;
	}

	// Read and check the metadata
	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();
	if (!doc["metadata"].IsObject() || str != "itpskel" || ver != 1)
	{
		DbgAssert(false, "Skeleton File Invalid Metadata");
		return false;
	}

	// Read and check bone count
	if (GetIntFromJSON(doc, "bonecount", mBoneNum) == false || mBoneNum < 0)
	{
		DbgAssert(false, "Skeleton File Invalid Bone Count");
		return false;
	}

	// Read bone array
	const rapidjson::Value& boneArray = doc["bones"];
	if (!boneArray.IsArray())
	{
		DbgAssert(false, "Skeleton File Invalid Bone Array");
		return false;
	}

	// Read each bone 
	std::string boneName;
	int parentIndex;
	Quaternion rotation;
	Vector3 translation;
	Skeleton::Bone bone;
	
	for (unsigned int i = 0; i < boneArray.Size(); i++)
	{
		if (GetStringFromJSON(boneArray[i], "name", boneName) == false)
		{
			DbgAssert(false, "Skeleton File Invalid Bone -> Name");
			return false;
		}

		if (GetIntFromJSON(boneArray[i], "parent", parentIndex) == false)
		{
			DbgAssert(false, "Skeleton File Invalid Bone -> Parent Index");
			return false;
		}

		if (GetQuaternionFromJSON(boneArray[i]["bindpose"], "rot", rotation) == false)
		{
			DbgAssert(false, "Skeleton File Invalid Bone -> Bind Pose -> Rotation");
			return false;
		}

		if (GetVectorFromJSON(boneArray[i]["bindpose"], "trans", translation) == false)
		{
			DbgAssert(false, "Skeleton File Invalid Bone -> Bind Pose -> Translation");
			return false;
		}

		bone.mName = boneName;
		bone.mParentIndex = parentIndex;
		bone.mTransform = BoneTransform(rotation, translation);
		mBones.push_back(bone);
	}

	ComputeGlobalInvBindPose();

	return true;
}

Skeleton* Skeleton::StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager)
{
	Skeleton* pSkeleton = new Skeleton();
	if (pSkeleton->Load(fileName) == false)
	{
		delete pSkeleton;
		return new Skeleton();
	}

	return pSkeleton;
}

size_t Skeleton::GetNumBones() const
{
	return mBoneNum;
};

const Skeleton::Bone& Skeleton::GetBone(size_t idx) const
{
	return mBones[idx];
};

const std::vector<Skeleton::Bone>& Skeleton::GetBones() const
{
	return mBones;
};

const std::vector<Matrix4>& Skeleton::GetGlobalInvBindPoses() const
{
	return mInverseBindPoseMatrices;
};