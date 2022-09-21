#include "stdafx.h"
#include "assetManager.h"


AssetManager::AssetManager()
{
	mShaderCache = new AssetCache<Shader>(this);
	mTextureCache = new AssetCache<Texture>(this);
	mMeshCache = new AssetCache<Mesh>(this);
	mMaterialCache = new AssetCache<Material>(this);
	mSkeletonCache = new AssetCache<Skeleton>(this);
	mAnimationCache = new AssetCache<Animation>(this);
}

AssetManager::~AssetManager()
{

}

void AssetManager::Clear()
{
	// Clear the shader cache for the leak detection
	mShaderCache->Clear();
	mTextureCache->Clear();
	mMeshCache->Clear();
	mMaterialCache->Clear();
	mAnimationCache->Clear();
	mSkeletonCache->Clear();
}

Shader* AssetManager::GetShader(const std::wstring& shaderName)
{
	return mShaderCache->Get(shaderName);
}

void AssetManager::SetShader(const std::wstring& shaderName, Shader* pShader)
{
	mShaderCache->Cache(shaderName, pShader);
}

Texture* AssetManager::LoadTexture(const std::wstring& fileName)
{
	Texture* texture = mTextureCache->Load(fileName);
	texture->Load(fileName.c_str());
	return texture;
}

Mesh* AssetManager::LoadMesh(const std::wstring& fileName)
{
	return mMeshCache->Load(fileName);
}

Material* AssetManager::LoadMaterial(const std::wstring& fileName)
{
	return mMaterialCache->Load(fileName);
}

Skeleton* AssetManager::LoadSkeleton(const std::wstring& fileName)
{
	return mSkeletonCache->Load(fileName);
}

Animation* AssetManager::LoadAnimation(const std::wstring& fileName)
{
	return mAnimationCache->Load(fileName);
}