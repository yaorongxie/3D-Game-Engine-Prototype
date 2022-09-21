#pragma once
#include "Graphics.h"
#include "engineMath.h"

struct PerCameraConstants
{
	Matrix4 c_viewProj;
	// Make float3 as a Vector4 because we need extra 4 bytes for padding
	Vector3 c_cameraPosition;
	float padding;
};

class Camera
{
public:
	Camera(Graphics* graphics);
	virtual ~Camera();
	void SetActive();
	void SetWorldToCameraMatrix(Matrix4 matrix) { mWorldToCameraMatrix = matrix; };

protected:
	Graphics* mGraphics;
	ID3D11DeviceContext* mDeviceContext;
	ID3D11Buffer* mConstantBuffer;
	Matrix4 mWorldToCameraMatrix;
	Matrix4 mProjectionMatrix;
	PerCameraConstants mCameraConstants;

private:

};

