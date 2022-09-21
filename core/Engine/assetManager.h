#pragma once
#include "assetCache.h"
#include "Shader.h"
#include "texture.h"
#include "Material.h"
#include "mesh.h"
#include "Skeleton.h"
#include "Animation.h"

class AssetManager
{
public:
    AssetManager();
    ~AssetManager();
    void Clear();
    Shader* GetShader(const std::wstring& shaderName);
    void SetShader(const std::wstring& shaderName, Shader* pShader);
    Texture* LoadTexture(const std::wstring& fileName);
    Mesh* LoadMesh(const std::wstring& fileName);
    Material* LoadMaterial(const std::wstring& fileName);
    Skeleton* LoadSkeleton(const std::wstring& fileName);
    Animation* LoadAnimation(const std::wstring& fileName);
  
private:
    AssetCache<Shader>* mShaderCache;
    AssetCache<Texture>* mTextureCache;
    AssetCache<Mesh>* mMeshCache;
    AssetCache<Material>* mMaterialCache;
    AssetCache<Skeleton>* mSkeletonCache;
    AssetCache<Animation>* mAnimationCache;
};
