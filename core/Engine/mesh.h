#pragma once

#include "engineMath.h"
#include "Graphics.h"

class AssetManager;
class Material;
class VertexBuffer;
class Shader;

class Mesh
{
public:
	Mesh(AssetManager* pAssetManager, const VertexBuffer* pVertexBuffer, Material* pMaterial);
	~Mesh();
	void Draw() const;
	bool Load(const WCHAR* fileName, AssetManager* pAssetManager);
	static Mesh* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);
	bool IsSkinned() const;

protected:
	AssetManager* mAssetManager;
	Material* mMaterial;
	const VertexBuffer* mVertexBuffer;	
	bool mIsSkinned;
};