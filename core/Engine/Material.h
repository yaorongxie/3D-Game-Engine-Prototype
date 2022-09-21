#pragma once
#include "engineMath.h"
#include "Graphics.h"

class Shader;
class Texture;
class AssetManager;

struct MaterialConstants
{
	Vector3 mDiffuseColor;
	char padding[4];
	Vector3 mSpecularColor;
	float mSpecularPower;
};

class Material
{
public:
	Material(Graphics* graphics);
	~Material();
	void SetActive();
	void SetShader(const Shader* shader);
	void SetTexture(int slot, const Texture* texture);
	void SetDiffuseColor(const Vector3& color);
	void SetSpecularColor(const Vector3& color);
	void SetSpecularPower(float power);
	MaterialConstants getMaterialConstants();
	bool Load(const WCHAR* fileName, AssetManager* pAssetManager);
	static Material* StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager);

private:
	void LoadShader(AssetManager* pAssetManager, std::wstring shaderName);

	Graphics* mGraphics;
	const Shader* mShader;
	const Texture* mTextures[Graphics::TEXTURE_SLOT_TOTAL];
	ID3D11Buffer* mConstantBuffer;
	MaterialConstants mMaterialConstants;
};

