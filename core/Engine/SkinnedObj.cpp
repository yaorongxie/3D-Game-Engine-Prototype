#include "stdafx.h"
#include "SkinnedObj.h"

SkinnedObj::SkinnedObj(const Mesh* mesh) : RenderObj(mesh)
{
	for (unsigned int i = 0; i < MAX_SKELETON_BONES; i++)
	{
		mSkinConstants.skinMatrices[i] = Matrix4::Identity;
	}

	mSkinConstantBuffer = mGraphics->CreateGraphicsBuffer(&mSkinConstants, sizeof(SkinConstants),
		D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

SkinnedObj::~SkinnedObj()
{
	mSkinConstantBuffer->Release();
}

void SkinnedObj::Draw()
{
	// Upload skinned constants buffer to vertex shader and pixel shader
	mGraphics->UploadBuffer(mSkinConstantBuffer, &mSkinConstants, sizeof(SkinConstants));
	mDeviceContext->VSSetConstantBuffers(mGraphics->CONSTANT_BUFFER_SKINNED, 1, &mSkinConstantBuffer);

	RenderObj::Draw();
}