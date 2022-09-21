#pragma once
#include "Graphics.h"
#include "Lights.h"
using namespace Lights;

class Shader;
class RenderObj;
class Camera;
class Texture;
class Material;
class AssetManager;
class Mesh;
class Physics;
class JobManager;

class Game
{
public:
    Game();
    ~Game();

    void Init(HWND hWnd, float width, float height);
    void Shutdown();
	void Update(float deltaTime);
    void RenderFrame();

	void OnKeyDown(uint32_t key);
	void OnKeyUp(uint32_t key);
	bool IsKeyHeld(uint32_t key) const;

	Lights::PointLightData* AllocateLight();
	void FreeLight(Lights::PointLightData* pLight);
	void SetAmbientLight(const Vector3& color);
	const Vector3& GetAmbientLight() const;

	AssetManager* GetAssetManager() { return mAssetManager; };
	Camera* GetCamera() { return mCamera; };
	Physics* GetPhysics() { return mPhysics; };

private:
	std::unordered_map<uint32_t, bool> m_keyIsHeld;
	Graphics mGraphics;
	Camera* mCamera;
	Physics* mPhysics;
	Texture* mFullSizedOffScreenTexture;
	ID3D11RenderTargetView* mFullSizedOffScreenBuffer;
	Texture* mHalfSizedOffScreenTexture;
	ID3D11RenderTargetView* mHalfSizedOffScreenBuffer;
	Texture* mQuadSizedOffScreenTextureOne;
	ID3D11RenderTargetView* mQuadSizedOffScreenBufferOne;
	Texture* mQuadSizedOffScreenTextureTwo;
	ID3D11RenderTargetView* mQuadSizedOffScreenBufferTwo;
	AssetManager* mAssetManager;
	ID3D11Buffer* mLightBuffer;
	std::vector<RenderObj*> mRenderObjects;
	ID3D11BlendState* opaqueBlendState;
	ID3D11BlendState* additiveBlendState;
	Lights::LightingConstants mLightConstants;
	ID3D11Buffer* mOffScreenVertexBuffer;
	unsigned int mOffScreenVertStride = 0;
	unsigned int mOffScreenVertCount = 0;
	unsigned int mOffScreenOffset = 0;

	bool LoadLevel(const WCHAR* fileName);
};