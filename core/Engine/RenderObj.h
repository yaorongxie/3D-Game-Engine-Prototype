#pragma once
#include "VertexBuffer.h"

class Material;
class Mesh;
class Component;

struct PerObjectConstants
{
	Matrix4 modelToWorldMatrix;
};

class RenderObj
{
public:
	RenderObj(const Mesh* mesh);
	virtual ~RenderObj();
	virtual void Draw();
	PerObjectConstants GetObjectConstants();
	void SetObjectConstants(PerObjectConstants newVal);
	void AddComponent(Component* pComp);
	void SetPosition(Vector3 pos) { mPosition = pos; };
	Vector3 GetPosition() { return mPosition; };
	virtual void Update(float deltaTime);

protected:
	RenderObj() {};

	Graphics* mGraphics;
	ID3D11DeviceContext* mDeviceContext;
	const Mesh* mMesh;
	ID3D11Buffer* mObjectBuffer;
	std::vector<Component*> mComponents;

	Vector3 mPosition;
	PerObjectConstants mObjectConstants;
};

