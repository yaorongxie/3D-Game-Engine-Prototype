#include "stdafx.h"
#include "Camera.h"

Camera::Camera(Graphics* graphics)
{
	mGraphics = graphics;
	mDeviceContext = mGraphics->GetDeviceContext();

	mWorldToCameraMatrix = Matrix4::CreateTranslation(Vector3(500.0f, 0.0f, 0.0f));

	mProjectionMatrix = Matrix4::CreateRotationY(-Math::PiOver2)
		* Matrix4::CreateRotationZ(-Math::PiOver2)
		* Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
			mGraphics->GetScreenWidth(), mGraphics->GetScreenHeight(),
			25.0f, 10000.0f);

	mConstantBuffer = mGraphics->CreateGraphicsBuffer(&mCameraConstants, sizeof(PerCameraConstants), 
		D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

Camera::~Camera()
{
	mGraphics = nullptr;
	mDeviceContext = nullptr;
	mConstantBuffer->Release();
}

void Camera::SetActive()
{
	mCameraConstants.c_viewProj = mWorldToCameraMatrix * mProjectionMatrix;

	Matrix4 cameraToWorldMatrix(mWorldToCameraMatrix);
	cameraToWorldMatrix.Invert();
	mCameraConstants.c_cameraPosition = cameraToWorldMatrix.GetTranslation();
	
	// Upload to constant buffers
	mGraphics->UploadBuffer(mConstantBuffer, &mCameraConstants, sizeof(PerCameraConstants));
	mDeviceContext->VSSetConstantBuffers(mGraphics->CONSTANT_BUFFER_CAMERA, 1, &mConstantBuffer);
	mDeviceContext->PSSetConstantBuffers(mGraphics->CONSTANT_BUFFER_CAMERA, 1, &mConstantBuffer);
}