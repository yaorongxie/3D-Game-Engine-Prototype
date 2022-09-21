#include "stdafx.h"
#include "Animation.h"
#include "Skeleton.h"
#include "assetManager.h"
#include "jsonUtil.h"
#include "stringUtil.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>
#include "Profiler.h"

bool Animation::Load(const WCHAR* fileName)
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
		DbgAssert(false, "Unable to open Animation file");
		return false;
	}

	// Read and check the metadata
	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();
	if (!doc["metadata"].IsObject() || str != "itpanim" || ver != 2)
	{
		DbgAssert(false, "Animation File Invalid Metadata");
		return false;
	}

	// Read the number of frames
	if (GetUintFromJSON(doc["sequence"], "frames", numFrames) == false || numFrames < 0)
	{
		DbgAssert(false, "Animation File Invalid Frames");
		return false;
	}

	// Read the animation length
	if (GetFloatFromJSON(doc["sequence"], "length", mLength) == false || mLength < 0)
	{
		DbgAssert(false, "Animation File Invalid Length");
		return false;
	}

	// Read the number of bones
	if (GetUintFromJSON(doc["sequence"], "bonecount", numBones) == false || numBones < 0)
	{
		DbgAssert(false, "Animation File Invalid Bone Count");
		return false;
	}
	mAnimationData.clear();
	mAnimationData.resize(numBones);

	// Read tracks array
	const rapidjson::Value& tracksArray = doc["sequence"]["tracks"];
	if (!tracksArray.IsArray())
	{
		DbgAssert(false, "Animation File Invalid Tracks Array");
		return false;
	}
	// Read each track
	int boneIndex;
	for (unsigned int i = 0; i < tracksArray.Size(); i++)
	{
		if (GetIntFromJSON(tracksArray[i], "bone", boneIndex) == false)
		{
			DbgAssert(false, "Animation File Invalid Tracks Array -> Bone Index");
			return false;
		}

		const rapidjson::Value& transformArray = tracksArray[i]["transforms"];
		if (!transformArray.IsArray())
		{
			DbgAssert(false, "Animation File Invalid Tracks Array -> Transforms");
			return false;
		}

		for (unsigned int j = 0; j < transformArray.Size(); j++)
		{
			Quaternion rotation;
			if (GetQuaternionFromJSON(transformArray[j], "rot", rotation) == false)
			{
				DbgAssert(false, "Animation File Invalid Tracks Array -> Transforms -> Rotation");
				return false;
			}

			Vector3 translation;
			if (GetVectorFromJSON(transformArray[j], "trans", translation) == false)
			{
				DbgAssert(false, "Animation File Invalid Tracks Array -> Transforms -> Translation");
				return false;
			}

			BoneTransform boneTransform(rotation, translation);
			mAnimationData[boneIndex].push_back(boneTransform);
		}
	}

	float time = GetLength() / (GetNumFrames() - 1);
	for (unsigned int i = 0; i < GetNumFrames(); i++)
	{
		mFrameTime.push_back(time * static_cast<float>(i));
	}

	return true;
}

Animation* Animation::StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager)
{
	Animation* pAnimation = new Animation();
	if (pAnimation->Load(fileName) == false)
	{
		delete pAnimation;
		return new Animation();
	}

	return pAnimation;
}

void Animation::GetGlobalPoseAtTime(std::vector<Matrix4>& outPoses, const Skeleton* inSkeleton, float inTime) const
{
	size_t frameAIndex = 0;
	size_t frameBIndex = 0;
	for (size_t i = 0; i < mFrameTime.size(); i++)
	{
		if (inTime < mFrameTime[i])
		{
			frameAIndex = i - 1;
			frameBIndex = i;
			break;
		}
	}
	float ratio = (inTime - mFrameTime[frameAIndex]) / (mFrameTime[frameBIndex] - mFrameTime[frameAIndex]);

	std::vector<Skeleton::Bone> const& bones = inSkeleton->GetBones();
	Matrix4 globalPoseMatrix;
	int parentIndex;

	for (unsigned int i = 0; i < mAnimationData.size(); i++)
	{
		// This bone doesn't have key frames
		if (mAnimationData[i].size() == 0)
		{
			globalPoseMatrix = Matrix4::Identity;
		}
		else
		{
			globalPoseMatrix = BoneTransform::Interpolate(mAnimationData[i][frameAIndex],
				mAnimationData[i][frameBIndex], ratio).ToMatrix();

			parentIndex = bones[i].mParentIndex;

			// Walk the parent hierachy to transform from its bone space to the world space
			if (parentIndex != -1)
			{
				globalPoseMatrix *= outPoses[parentIndex];
			}

			outPoses[i] = globalPoseMatrix;
		}
	}
}

uint32_t Animation::GetNumBones() const
{
	return numBones;
}

uint32_t Animation::GetNumFrames() const
{
	return numFrames;
}

float Animation::GetLength() const
{
	return mLength;
}