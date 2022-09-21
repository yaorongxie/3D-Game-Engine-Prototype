#include "stdafx.h"
#include "Material.h"
#include "Shader.h"
#include "texture.h"
#include "assetManager.h"
#include "jsonUtil.h"
#include "stringUtil.h"
#include"VertexFormat.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>

/****** ADD SHADER LAYOUT HERE AND UPDATE LOAD SHADER FUNCTION ******/
// Layout for the normal shader (without texture)
static D3D11_INPUT_ELEMENT_DESC meshLayout[] =
{
	// 1st parameter -- semantics, which identifies how the data will be used
	// 2nd parameter -- semantics index
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	offsetof(MeshLayout, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	offsetof(MeshLayout, color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

// Layout for the texture shader (with texture)
static D3D11_INPUT_ELEMENT_DESC basicMeshLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	offsetof(BasicMeshLayout, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	offsetof(BasicMeshLayout, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
	offsetof(BasicMeshLayout, color), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	offsetof(BasicMeshLayout, textCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

// Layout for the phong and unlit shader (with texture)
static D3D11_INPUT_ELEMENT_DESC phongUnlitLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	offsetof(PhongUnlitLayout, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	offsetof(PhongUnlitLayout, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	offsetof(PhongUnlitLayout, textCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

// Layout for the skinned shader
static D3D11_INPUT_ELEMENT_DESC skinnedLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	offsetof(SkinnedLayout, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	offsetof(SkinnedLayout, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "BONE", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0,
	offsetof(SkinnedLayout, boneIndices), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "WEIGHT", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0,
	offsetof(SkinnedLayout, weights), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	offsetof(SkinnedLayout, textCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

// Layout for the normal shader
static D3D11_INPUT_ELEMENT_DESC normalLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	offsetof(NormalLayout, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	offsetof(NormalLayout, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	offsetof(NormalLayout, tangent), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	offsetof(NormalLayout, textCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

Material::Material(Graphics* graphics)
{
	mGraphics = graphics;

	// Create material constants buffer
	mConstantBuffer = mGraphics->CreateGraphicsBuffer(&mMaterialConstants, sizeof(MaterialConstants),
		D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

Material::~Material()
{
	mGraphics = nullptr;

	mShader = nullptr;
	
	for (unsigned int i = 0; i < Graphics::TEXTURE_SLOT_TOTAL; i++)
	{
		mTextures[i] = nullptr;
	}

	mConstantBuffer->Release();
}

void Material::SetActive()
{
	// Set material shader to active
	if (mShader != nullptr)
	{
		mShader->SetActive();
	}

	// Set every texture to active
	for (int slot = 0; slot < Graphics::TEXTURE_SLOT_TOTAL; slot++)
	{
		if (mTextures[slot] != nullptr)
		{
			mTextures[slot]->SetActive(slot);
		}	
	}

	// Upload material constants buffer
	mGraphics->UploadBuffer(mConstantBuffer, &mMaterialConstants, sizeof(MaterialConstants));
	mGraphics->GetDeviceContext()->VSSetConstantBuffers(mGraphics->CONSTANT_BUFFER_MATERIAL, 1, &mConstantBuffer);
	mGraphics->GetDeviceContext()->PSSetConstantBuffers(mGraphics->CONSTANT_BUFFER_MATERIAL, 1, &mConstantBuffer);
}

void Material::SetShader(const Shader* shader)
{
	mShader = shader;
}

void Material::SetTexture(int slot, const Texture* texture)
{
	// Make sure slot is valid
	if (slot >= 0 && slot < Graphics::TEXTURE_SLOT_TOTAL)
	{
		mTextures[slot] = texture;
	}
}

void Material::SetDiffuseColor(const Vector3& color)
{
	mMaterialConstants.mDiffuseColor.x = color.x;
	mMaterialConstants.mDiffuseColor.y = color.y;
	mMaterialConstants.mDiffuseColor.z = color.z;
}

void Material::SetSpecularColor(const Vector3& color)
{
	mMaterialConstants.mSpecularColor.x = color.x;
	mMaterialConstants.mSpecularColor.y = color.y;
	mMaterialConstants.mSpecularColor.z = color.z;
}

void Material::SetSpecularPower(float power)
{
	mMaterialConstants.mSpecularPower = power;
}

MaterialConstants Material::getMaterialConstants()
{
	return mMaterialConstants;
}

bool Material::Load(const WCHAR* fileName, AssetManager* pAssetManager)
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

	if (!doc.IsObject())
	{
		DbgAssert(false, "Unable to open Material file");
		return false;
	}

	// Check the metadata
	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();
	if (!doc["metadata"].IsObject() || str != "itpmat" || ver != 1)
	{
		DbgAssert(false, "Material File Invalid Metadata");
		return false;
	}

	// Load and set shader
	std::string shaderName;
	if (GetStringFromJSON(doc, "shader", shaderName) == false)
	{
		DbgAssert(false, "Material File Incorrect Shader");
		return false;
	}
	shaderName = "Shaders/" + shaderName + ".hlsl";
	std::wstring wsShaderName(shaderName.begin(), shaderName.end());
	LoadShader(pAssetManager, wsShaderName);
	SetShader(pAssetManager->GetShader(wsShaderName));

	// Read the texture array
	const rapidjson::Value& textureArray = doc["textures"];
	if (!textureArray.IsArray())
	{
		DbgAssert(false, "Material File Invalid Texture Array");
		return false;
	}
	// Read individual texture
	std::string textureName;
	for (unsigned int i = 0; i < textureArray.Size(); i++)
	{
		textureName = textureArray[i].GetString();
		std::wstring wsTexture(textureName.begin(), textureName.end());

		// Load and Set texture
		if (wsTexture.find(L"normal") != std::string::npos)
		{
			SetTexture(Graphics::TEXTURE_SLOT_NORMAL, pAssetManager->LoadTexture(wsTexture));
		}
		else
		{
			SetTexture(Graphics::TEXTURE_SLOT_DIFFUSE, pAssetManager->LoadTexture(wsTexture));	
		}
	}

	// Read and set diffuse color
	Vector3 diffuseColor;
	GetVectorFromJSON(doc, "diffuseColor", diffuseColor);
	SetDiffuseColor(diffuseColor);

	// Read and set specular color
	Vector3 specularColor;
	GetVectorFromJSON(doc, "specularColor", specularColor);
	SetSpecularColor(specularColor);

	// Read and set specular power
	float specularPower = 0.0f;
	GetFloatFromJSON(doc, "specularPower", specularPower);
	SetSpecularPower(specularPower);

	return true;
}

Material* Material::StaticLoad(const WCHAR* fileName, AssetManager* pAssetManager)
{
	Material* pMaterial = new Material(Graphics::Get());
	if (pMaterial->Load(fileName, pAssetManager) == false)
	{
		delete pMaterial;
		return new Material(Graphics::Get());
	}

	return pMaterial;
};

void Material::LoadShader(AssetManager* pAssetManager, std::wstring shaderName)
{
	// This shader is not cached yet
	if (pAssetManager->GetShader(shaderName) == nullptr)
	{
		// Cache this shader
		pAssetManager->SetShader(shaderName, new Shader());

		// Load the corresponding shader file
		if (shaderName == L"Shaders/Mesh.hlsl" && pAssetManager->GetShader(L"Shaders/Mesh.hlsl") != nullptr)
		{
			pAssetManager->GetShader(L"Shaders/Mesh.hlsl")->Load(L"Shaders/Mesh.hlsl", meshLayout, ARRAY_SIZE(meshLayout));
		}
		else if (shaderName == L"Shaders/BasicMesh.hlsl" && pAssetManager->GetShader(L"Shaders/BasicMesh.hlsl") != nullptr)
		{
			pAssetManager->GetShader(L"Shaders/BasicMesh.hlsl")->Load(L"Shaders/BasicMesh.hlsl", basicMeshLayout, ARRAY_SIZE(basicMeshLayout));
		}
		else if (shaderName == (L"Shaders/Phong.hlsl") && pAssetManager->GetShader(L"Shaders/Phong.hlsl") != nullptr)
		{
			pAssetManager->GetShader(L"Shaders/Phong.hlsl")->Load(L"Shaders/Phong.hlsl", phongUnlitLayout, ARRAY_SIZE(phongUnlitLayout));
		}
		else if (shaderName == (L"Shaders/Unlit.hlsl") && pAssetManager->GetShader(L"Shaders/Unlit.hlsl") != nullptr)
		{
			pAssetManager->GetShader(L"Shaders/Unlit.hlsl")->Load(L"Shaders/Unlit.hlsl", phongUnlitLayout, ARRAY_SIZE(phongUnlitLayout));
		}
		else if (shaderName == (L"Shaders/Skinned.hlsl") && pAssetManager->GetShader(L"Shaders/Skinned.hlsl") != nullptr)
		{
			pAssetManager->GetShader(L"Shaders/Skinned.hlsl")->Load(L"Shaders/Skinned.hlsl", skinnedLayout, ARRAY_SIZE(skinnedLayout));
		}
		else if (shaderName == (L"Shaders/Normal.hlsl") && pAssetManager->GetShader(L"Shaders/Normal.hlsl") != nullptr)
		{
			pAssetManager->GetShader(L"Shaders/Normal.hlsl")->Load(L"Shaders/Normal.hlsl", normalLayout, ARRAY_SIZE(normalLayout));
		}
	}
}