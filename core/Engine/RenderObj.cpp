#include "stdafx.h"
#include "RenderObj.h"
#include "Material.h"
#include "mesh.h"
#include "Component.h"
#include "Profiler.h"

RenderObj::RenderObj(const Mesh* mesh) : mMesh(nullptr)
{
	mGraphics = Graphics::Get();

	mMesh = mesh;

	mDeviceContext = mGraphics->GetDeviceContext();

	mObjectConstants.modelToWorldMatrix = Matrix4::CreateRotationZ(Math::ToRadians(45.0f));

	mObjectBuffer = mGraphics->CreateGraphicsBuffer(&mObjectConstants, sizeof(PerObjectConstants),
		D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

RenderObj::~RenderObj()
{
	mGraphics = nullptr;

	mDeviceContext = nullptr;

	mMesh = nullptr;

	mObjectBuffer->Release();
	mObjectBuffer = nullptr;

	for (unsigned int i = 0; i < mComponents.size(); i++)
	{
		delete mComponents[i];
	}
	mComponents.clear();
}

void RenderObj::Draw()
{
	// Upload object constants buffer to vertex shader and pixel shader
	mGraphics->UploadBuffer(mObjectBuffer, &mObjectConstants, sizeof(PerObjectConstants));
	mDeviceContext->VSSetConstantBuffers(mGraphics->CONSTANT_BUFFER_RENDEROBJ, 1, &mObjectBuffer);

	{
		PROFILE_SCOPE(MeshDraw);
		mMesh->Draw();
	}
}

PerObjectConstants RenderObj::GetObjectConstants()
{
	return mObjectConstants;
};

void RenderObj::SetObjectConstants(PerObjectConstants newVal)
{
	mObjectConstants.modelToWorldMatrix = newVal.modelToWorldMatrix;
}

void RenderObj::AddComponent(Component* pComp)
{
	mComponents.push_back(pComp);
}

void RenderObj::Update(float deltaTime)
{
	{
		PROFILE_SCOPE(ComponentUpdate);

		for (int i = 0; i < mComponents.size(); i++)
		{
			mComponents[i]->Update(deltaTime);
		}
	}
}