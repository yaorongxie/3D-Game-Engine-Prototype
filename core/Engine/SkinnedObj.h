#pragma once
#include "RenderObj.h"

#define MAX_SKELETON_BONES 80

struct SkinConstants
{
	Matrix4 skinMatrices[MAX_SKELETON_BONES];
};

class SkinnedObj : public RenderObj
{
public:
	SkinnedObj(const Mesh* mesh);
	~SkinnedObj();
	void Draw() override;

	SkinConstants mSkinConstants;

protected:
	ID3D11Buffer* mSkinConstantBuffer;
};

