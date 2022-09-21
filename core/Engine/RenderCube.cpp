#include "stdafx.h"
#include "RenderCube.h"
#include "mesh.h"
#include "assetManager.h"

RenderCube::RenderCube(AssetManager* pAssetManager, const Shader* pShader, const Texture* pTex)
{
	mGraphics = Graphics::Get();

	mMesh = Mesh::StaticLoad(L"Assets/Meshes/Cube.itpmesh3", pAssetManager);

	mDeviceContext = mGraphics->GetDeviceContext();

	mObjectConstants.modelToWorldMatrix = Matrix4::CreateRotationZ(Math::ToRadians(45.0f));

	mObjectBuffer = mGraphics->CreateGraphicsBuffer(&mObjectConstants, sizeof(PerObjectConstants),
		D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

RenderCube::~RenderCube()
{
	mGraphics = nullptr;
	delete mMesh;
}
