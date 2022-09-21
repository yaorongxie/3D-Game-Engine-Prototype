#include "stdafx.h"
#include "VertexBuffer.h"


VertexBuffer::VertexBuffer(Graphics* graphics, 
	const void* vertData, uint32_t vertCount, uint32_t vertStride, 
	const void* indexData, uint32_t indexCount, uint32_t indexStride)
{
	mGraphics = graphics;
	mDeviceContext = mGraphics->GetDeviceContext();
	mOffset = 0;

	mVertCount = vertCount;
	mVertStride = vertStride;

	mIndexCount = indexCount;
	mIndexStride = indexStride;

	// Choose 16-bit or 32-bit format based on index stride
	if (mIndexStride == 2)
	{
		mIndexFormat = DXGI_FORMAT_R16_UINT;
	}
	else
	{
		mIndexFormat = DXGI_FORMAT_R32_UINT;
	}

	// Size of vertData = vertCount * VertStride
	// Don't use size of vertData, because vertData is a pointer to the 
	// first element of the original array, which is 8 bytes!!!
	mVertexBuffer = mGraphics->CreateGraphicsBuffer(vertData, vertCount * vertStride,
		D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

	mIndexBuffer = mGraphics->CreateGraphicsBuffer(indexData, indexCount * indexStride,
		D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

VertexBuffer::~VertexBuffer()
{
	mGraphics = nullptr;
	mDeviceContext = nullptr;

	mVertexBuffer->Release();
	mVertexBuffer = nullptr;

	mIndexBuffer->Release();
	mIndexBuffer = nullptr;
}

void VertexBuffer::Draw() const
{
	mDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &mVertStride, &mOffset);
	mDeviceContext->IASetIndexBuffer(mIndexBuffer, mIndexFormat, mOffset);
	mDeviceContext->DrawIndexed(mIndexCount, 0, 0);
}