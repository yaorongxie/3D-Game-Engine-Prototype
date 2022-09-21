#pragma once
#include "Graphics.h"
#include "VertexFormat.h"

class VertexBuffer
{
public:
	VertexBuffer(Graphics* graphics,
		const void* vertexData, uint32_t vertexCount, uint32_t vertexStride,
		const void* indexData, uint32_t indexCount, uint32_t indexStride);
	~VertexBuffer();
	void Draw() const;

private:
	Graphics* mGraphics;
	ID3D11DeviceContext* mDeviceContext;

	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;

	uint32_t mOffset;
	uint32_t mVertCount;
	uint32_t mVertStride;
	uint32_t mIndexCount;
	uint32_t mIndexStride;
	DXGI_FORMAT mIndexFormat;
};

