#include "stdafx.h"
#include "Game.h"
#include "engineMath.h"
#include "Graphics.h"
#include "Shader.h"
#include "stringUtil.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>
#include "RenderObj.h"
#include "RenderCube.h"
#include "SkinnedObj.h"
#include "Camera.h"
#include "Material.h"
#include "texture.h"
#include "assetManager.h"
#include "mesh.h"
#include "jsonUtil.h"
#include "Skeleton.h"
#include "Physics.h"
#include "Components\PointLight.h"
#include "Components\Character.h"
#include "Components\player.h"
#include "Components\followCam.h"
#include "Components\SimpleRotate.h"
#include "CollisionBox.h"
#include "Profiler.h"
#include "Job.h"
#include "JobManager.h"

static float rotationAngle = 0.0f;

// Layout for the copy shader
static D3D11_INPUT_ELEMENT_DESC copyLayout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
	offsetof(CopyLayout, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },

	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
	offsetof(CopyLayout, textCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 }
};

static CopyLayout copyVertices[] =
{
	{ Vector3(-1.0f, -1.0f, 0.0f), Vector2(0.0f, 1.0f) },
	{ Vector3(1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f) },
	{ Vector3(-1.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f) },
	
	{ Vector3(1.0f, 1.0f, 0.0f), Vector2(1.0f, 0.0f) },	
	{ Vector3(-1.0f, 1.0f, 0.0f), Vector2(0.0f, 0.0f) },
	{ Vector3(1.0f, -1.0f, 0.0f), Vector2(1.0f, 1.0f) }	
};

Game::Game()
{
	opaqueBlendState = nullptr;
	additiveBlendState = nullptr;
}

Game::~Game()
{

}

void Game::Init(HWND hWnd, float width, float height)
{
	// Initialize graphics
	mGraphics.InitD3D(hWnd, width, height);

	D3D11_RASTERIZER_DESC mRasterizerDesc;
	ZeroMemory(&mRasterizerDesc, sizeof(mRasterizerDesc));
	mRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	mRasterizerDesc.CullMode = D3D11_CULL_BACK;
	mRasterizerDesc.FrontCounterClockwise = true;
	ID3D11RasterizerState* mRasterizerState;
	ZeroMemory(&mRasterizerState, sizeof(mRasterizerState));
	mGraphics.GetDevice()->CreateRasterizerState(&mRasterizerDesc, &mRasterizerState);
	mGraphics.GetDeviceContext()->RSSetState(mRasterizerState);
	mRasterizerState->Release();

	// Create blend state
	opaqueBlendState = mGraphics.CreateBlendState(false, D3D11_BLEND_ONE, D3D11_BLEND_ZERO);
	additiveBlendState = mGraphics.CreateBlendState(true, D3D11_BLEND_ONE, D3D11_BLEND_ONE);

	// Create asset manager
	mAssetManager = new AssetManager();

	// Create full-sized render target
	mFullSizedOffScreenTexture = new Texture;
	mFullSizedOffScreenBuffer = mFullSizedOffScreenTexture->
		CreateRenderTarget(width, height, DXGI_FORMAT_R32G32B32A32_FLOAT);

	// Create half-sized render target
	mHalfSizedOffScreenTexture = new Texture;
	mHalfSizedOffScreenBuffer = mHalfSizedOffScreenTexture->
		CreateRenderTarget(0.5 * width, 0.5 * height, DXGI_FORMAT_R32G32B32A32_FLOAT);

	// Create two quarter-sized render targets
	mQuadSizedOffScreenTextureOne = new Texture;
	mQuadSizedOffScreenBufferOne = mQuadSizedOffScreenTextureOne->
		CreateRenderTarget(0.25 * width, 0.25 * height, DXGI_FORMAT_R32G32B32A32_FLOAT);
	mQuadSizedOffScreenTextureTwo = new Texture;
	mQuadSizedOffScreenBufferTwo = mQuadSizedOffScreenTextureTwo->
		CreateRenderTarget(0.25 * width, 0.25 * height, DXGI_FORMAT_R32G32B32A32_FLOAT);

	// Set and load copy shader
	mAssetManager->SetShader(L"Shaders/Copy.hlsl", new Shader());
	mAssetManager->GetShader(L"Shaders/Copy.hlsl")->Load(L"Shaders/Copy.hlsl", copyLayout, ARRAY_SIZE(copyLayout));

	// Set and load bloom mask shader
	mAssetManager->SetShader(L"Shaders/BloomMask.hlsl", new Shader());
	mAssetManager->GetShader(L"Shaders/BloomMask.hlsl")->Load(L"Shaders/BloomMask.hlsl", copyLayout, ARRAY_SIZE(copyLayout));

	// Set and load blur shaders
	mAssetManager->SetShader(L"Shaders/BlurX.hlsl", new Shader());
	mAssetManager->GetShader(L"Shaders/BlurX.hlsl")->Load(L"Shaders/BlurX.hlsl", copyLayout, ARRAY_SIZE(copyLayout));
	mAssetManager->SetShader(L"Shaders/BlurY.hlsl", new Shader());
	mAssetManager->GetShader(L"Shaders/BlurY.hlsl")->Load(L"Shaders/BlurY.hlsl", copyLayout, ARRAY_SIZE(copyLayout));

	// Create a vertex buffer for the copy shader
	mOffScreenVertexBuffer = mGraphics.CreateGraphicsBuffer(copyVertices, sizeof(copyVertices),
		D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
	mOffScreenVertStride = sizeof(copyVertices[0]);
	mOffScreenVertCount = sizeof(copyVertices) / sizeof(copyVertices[0]);

	// Create camera
	mCamera = new Camera(&mGraphics);

	// Create physics
	mPhysics = new Physics();

	// Create and begin the job manager
	JobManager::Get()->Begin();

	// Clean up the lighting constants buffer
	ZeroMemory(&mLightConstants, sizeof(mLightConstants));

	// Load game objects of the same level
	if (LoadLevel(L"Assets/Levels/Level11.itplevel") == false)
	{
		DbgAssert(false, "Fail to load game level");
	}

	// Load skeleton data from a skeleton file
	mAssetManager->LoadSkeleton(L"Assets/Anims/SK_Mannequin.itpskel");

	// Load animation data from a animation file
	mAssetManager->LoadAnimation(L"Assets/Anims/ThirdPersonRun.itpanim2");

	// Create the lighting buffer
	mLightBuffer = mGraphics.CreateGraphicsBuffer(&mLightConstants, sizeof(Lights::LightingConstants),
		D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

void Game::Shutdown()
{
	// Deallocate all resources
	opaqueBlendState->Release();
	opaqueBlendState = nullptr;

	additiveBlendState->Release();
	additiveBlendState = nullptr;

	delete mFullSizedOffScreenTexture;
	mFullSizedOffScreenTexture = nullptr;

	delete mHalfSizedOffScreenTexture;
	mHalfSizedOffScreenTexture = nullptr;

	delete mQuadSizedOffScreenTextureOne;
	mQuadSizedOffScreenTextureOne = nullptr;

	delete mQuadSizedOffScreenTextureTwo;
	mQuadSizedOffScreenTextureTwo = nullptr;

	mFullSizedOffScreenBuffer->Release();
	mFullSizedOffScreenBuffer = nullptr;

	mHalfSizedOffScreenBuffer->Release();
	mHalfSizedOffScreenBuffer = nullptr;

	mQuadSizedOffScreenBufferOne->Release();
	mQuadSizedOffScreenBufferOne = nullptr;

	mQuadSizedOffScreenBufferTwo->Release();
	mQuadSizedOffScreenBufferTwo = nullptr;

	mOffScreenVertexBuffer->Release();
	mOffScreenVertexBuffer = nullptr;

	delete mCamera;
	mCamera = nullptr;

	mAssetManager->Clear();
	mAssetManager = nullptr;

	mLightBuffer->Release();
	mLightBuffer = nullptr;

	for (int i = 0; i < mRenderObjects.size(); i++)
	{
		delete mRenderObjects[i];
	}
	mRenderObjects.clear();

	mGraphics.CleanD3D();

	JobManager::Get()->End();
}

void Game::Update(float deltaTime)
{
	PROFILE_SCOPE(GameUpdate);

	rotationAngle += (Math::Pi * deltaTime);

	{
		PROFILE_SCOPE(RenderObjsUpdate);

		for (int i = 0; i < mRenderObjects.size(); i++)
		{
			mRenderObjects[i]->Update(deltaTime);
		}
	}

	// Job manager wait for jobs
	JobManager::Get()->WaitForJobs();
}

void Game::RenderFrame()
{
	PROFILE_SCOPE(GameRender);

	// Set the render target
	// mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), mGraphics.getDepthStencilView());
	mGraphics.SetRenderTarget(mFullSizedOffScreenBuffer, mGraphics.getDepthStencilView());

	// Set blend state to opaque
	mGraphics.SetBlendState(opaqueBlendState);

	// Clear the render target
	Graphics::Color4 clearColor(0.0f, 0.2f, 0.4f, 1.0f);
	mGraphics.ClearRenderTarget(clearColor);

	// Clear the depth buffer
	mGraphics.ClearDepthBuffer(mGraphics.getDepthStencilView(), 1.0f);

	// Set the camera to active
	mCamera->SetActive();
	
	// Upload the lighting buffer
	mGraphics.UploadBuffer(mLightBuffer, &mLightConstants, sizeof(Lights::LightingConstants));
	// Set constant buffers in the vertex and pixel shader
	mGraphics.GetDeviceContext()->VSSetConstantBuffers(mGraphics.CONSTANT_BUFFER_LIGHTING, 1, &mLightBuffer);
	mGraphics.GetDeviceContext()->PSSetConstantBuffers(mGraphics.CONSTANT_BUFFER_LIGHTING, 1, &mLightBuffer);

	{
		PROFILE_SCOPE(RenderObjsDraw);

		// Draw objects
		for (int i = 0; i < mRenderObjects.size(); i++)
		{
			mRenderObjects[i]->Draw();
		}

		// Use BloomMask shader to draw that onto the half-by-half buffer
		mGraphics.SetViewport(0.0, 0.0, 0.5 * mGraphics.GetScreenWidth(), 0.5 * mGraphics.GetScreenHeight());
		mGraphics.SetRenderTarget(mHalfSizedOffScreenBuffer, nullptr);
		mAssetManager->GetShader(L"Shaders/BloomMask.hlsl")->SetActive();
		mFullSizedOffScreenTexture->SetActive(Graphics::TEXTURE_SLOT_OFFSCREEN);
		mGraphics.GetDeviceContext()->IASetVertexBuffers(0, 1, &mOffScreenVertexBuffer, &mOffScreenVertStride, &mOffScreenOffset);
		mGraphics.GetDeviceContext()->Draw(mOffScreenVertCount, 0);
		Graphics::Get()->SetActiveTexture(Graphics::TEXTURE_SLOT_OFFSCREEN, nullptr);

		// Use Copy shader to copy the half-by-half texture onto the quarter-by-quarter buffer
		mGraphics.SetViewport(0.0, 0.0, 0.25 * mGraphics.GetScreenWidth(), 0.25 * mGraphics.GetScreenHeight());
		mGraphics.SetRenderTarget(mQuadSizedOffScreenBufferOne, nullptr);
		mAssetManager->GetShader(L"Shaders/Copy.hlsl")->SetActive();
		mHalfSizedOffScreenTexture->SetActive(Graphics::TEXTURE_SLOT_OFFSCREEN);
		mGraphics.GetDeviceContext()->IASetVertexBuffers(0, 1, &mOffScreenVertexBuffer, &mOffScreenVertStride, &mOffScreenOffset);
		mGraphics.GetDeviceContext()->Draw(mOffScreenVertCount, 0);
		Graphics::Get()->SetActiveTexture(Graphics::TEXTURE_SLOT_OFFSCREEN, nullptr);

		// Use BlurX shaderto do the horizontal pass from quarter#1 onto quarter#2
		mGraphics.SetViewport(0.0, 0.0, 0.25 * mGraphics.GetScreenWidth(), 0.25 * mGraphics.GetScreenHeight());
		mGraphics.SetRenderTarget(mQuadSizedOffScreenBufferTwo, nullptr);
		mAssetManager->GetShader(L"Shaders/BlurX.hlsl")->SetActive();
		mQuadSizedOffScreenTextureOne->SetActive(Graphics::TEXTURE_SLOT_OFFSCREEN);
		mGraphics.GetDeviceContext()->IASetVertexBuffers(0, 1, &mOffScreenVertexBuffer, &mOffScreenVertStride, &mOffScreenOffset);
		mGraphics.GetDeviceContext()->Draw(mOffScreenVertCount, 0);
		Graphics::Get()->SetActiveTexture(Graphics::TEXTURE_SLOT_OFFSCREEN, nullptr);

		// Use BLurY shader to do the vertical pass from quarter#2 onto quarter#1
		mGraphics.SetViewport(0.0, 0.0, 0.25 * mGraphics.GetScreenWidth(), 0.25 * mGraphics.GetScreenHeight());
		mGraphics.SetRenderTarget(mQuadSizedOffScreenBufferOne, nullptr);
		mAssetManager->GetShader(L"Shaders/BlurY.hlsl")->SetActive();
		mQuadSizedOffScreenTextureTwo->SetActive(Graphics::TEXTURE_SLOT_OFFSCREEN);
		mGraphics.GetDeviceContext()->IASetVertexBuffers(0, 1, &mOffScreenVertexBuffer, &mOffScreenVertStride, &mOffScreenOffset);
		mGraphics.GetDeviceContext()->Draw(mOffScreenVertCount, 0);
		Graphics::Get()->SetActiveTexture(Graphics::TEXTURE_SLOT_OFFSCREEN, nullptr);

		// Use Copy shader to copy the blurry texture (1/4 resolution) to the back buffer --> SRC BLEND (pixel shader output)
		mGraphics.SetViewport(0.0, 0.0, mGraphics.GetScreenWidth(), mGraphics.GetScreenHeight());
		mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), nullptr);
		mAssetManager->GetShader(L"Shaders/Copy.hlsl")->SetActive();
		mQuadSizedOffScreenTextureOne->SetActive(Graphics::TEXTURE_SLOT_OFFSCREEN);
		mGraphics.GetDeviceContext()->IASetVertexBuffers(0, 1, &mOffScreenVertexBuffer, &mOffScreenVertStride, &mOffScreenOffset);
		mGraphics.GetDeviceContext()->Draw(mOffScreenVertCount, 0);
		Graphics::Get()->SetActiveTexture(Graphics::TEXTURE_SLOT_OFFSCREEN, nullptr);

		// Set blend state to Additive --> DST = SRC + DST;
		mGraphics.SetBlendState(additiveBlendState);

		// Use Copy shader to copy the original (full resolution) texture to the back buffer --> DST BLEND (pixel shader input)
		mGraphics.SetViewport(0.0, 0.0, mGraphics.GetScreenWidth(), mGraphics.GetScreenHeight());
		mGraphics.SetRenderTarget(mGraphics.GetBackBuffer(), nullptr);
		mAssetManager->GetShader(L"Shaders/Copy.hlsl")->SetActive();
		mFullSizedOffScreenTexture->SetActive(Graphics::TEXTURE_SLOT_OFFSCREEN);
		mGraphics.GetDeviceContext()->IASetVertexBuffers(0, 1, &mOffScreenVertexBuffer, &mOffScreenVertStride, &mOffScreenOffset);
		mGraphics.GetDeviceContext()->Draw(mOffScreenVertCount, 0);
		Graphics::Get()->SetActiveTexture(Graphics::TEXTURE_SLOT_OFFSCREEN, nullptr);
	}

	// End the current frame
	mGraphics.EndFrame();
}

void Game::OnKeyDown(uint32_t key)
{
	m_keyIsHeld[key] = true;
}

void Game::OnKeyUp(uint32_t key)
{
	m_keyIsHeld[key] = false;
}

bool Game::IsKeyHeld(uint32_t key) const
{
	const auto find = m_keyIsHeld.find(key);
	if (find != m_keyIsHeld.end())
		return find->second;
	return false;
}

bool Game::LoadLevel(const WCHAR* fileName)
{

	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		return false;
	}


	// Check the metadata
	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();
	if (!doc["metadata"].IsObject() ||
		str != "itplevel" ||
		ver != 2)
	{
		return false;
	}


	// Read camera position
	const rapidjson::Value& cameraDoc = doc["camera"];
	Vector3 cameraPosition;
	if (GetVectorFromJSON(cameraDoc, "position", cameraPosition) == false)
	{
		DbgAssert(false, "Level File Invalid Camera Position");
		return false;
	}
	// Read camera rotation
	Quaternion cameraRotation;
	if (GetQuaternionFromJSON(cameraDoc, "rotation", cameraRotation) == false)
	{
		DbgAssert(false, "Level File Invalid Camera Rotation");
		return false;
	}
	// Rotation and translation matrix
	Matrix4 rotationMatrix = Matrix4::CreateFromQuaternion(cameraRotation);
	Matrix4 translationMatrix = Matrix4::CreateTranslation(cameraPosition);
	// Camera-To-World Matrix
	Matrix4 temp = rotationMatrix * translationMatrix;
	// World-To-Camera Matrix
	temp.Invert();
	mCamera->SetWorldToCameraMatrix(temp);


	// Read and set lighting data
	const rapidjson::Value& lightingDataDoc = doc["lightingData"];
	Vector3 ambientLight;
	if (GetVectorFromJSON(lightingDataDoc, "ambient", ambientLight) == false)
	{
		DbgAssert(false, "Level File Invalid Ambient Light");
		return false;
	}
	SetAmbientLight(ambientLight);


	// Read and set render objects
	const rapidjson::Value& renderObjectsDoc = doc["renderObjects"];
	if (!doc["renderObjects"].IsArray() || doc["renderObjects"].Size() < 1)
	{
		DbgAssert(false, "Level File Invalid Render Object Array");
		return false;
	}
	for (unsigned int i = 0; i < renderObjectsDoc.Size(); i++)
	{
		if (!renderObjectsDoc[i].IsObject())
		{
			DbgAssert(false, "Level File Invalid Render Object");
			return false;
		}

		// Read render object position
		Vector3 renderObjPosition;
		if (GetVectorFromJSON(renderObjectsDoc[i], "position", renderObjPosition) == false)
		{
			DbgAssert(false, "Level File Invalid Render Object Position");
			return false;
		}
		// Read render object rotation
		Quaternion renderObjRotation;
		if (GetQuaternionFromJSON(renderObjectsDoc[i], "rotation", renderObjRotation) == false)
		{
			DbgAssert(false, "Level File Invalid Render Object Rotation");
			return false;
		}
		// Read render object scale
		float renderObjScale;
		if (GetFloatFromJSON(renderObjectsDoc[i], "scale", renderObjScale) == false)
		{
			DbgAssert(false, "Level File Invalid Render Object Scale");
			return false;
		}

		// Read render object mesh
		std::string renderObjMeshString;
		if (GetStringFromJSON(renderObjectsDoc[i], "mesh", renderObjMeshString) == false)
		{
			DbgAssert(false, "Level File Invalid Render Object Mesh");
			return false;
		}
		std::wstring renderObjMesh(renderObjMeshString.begin(), renderObjMeshString.end());

		// Compute the transform matrix of the render object
		Matrix4 scaleMatrix = Matrix4::CreateScale(renderObjScale);
		Matrix4 rotationMatrix = Matrix4::CreateFromQuaternion(renderObjRotation);
		Matrix4 translationMatrix = Matrix4::CreateTranslation(renderObjPosition);
		Matrix4 modelToWorldMatrix = scaleMatrix * rotationMatrix * translationMatrix;

		// Load the mesh of the render object and create a render object of the given data
		mAssetManager->LoadMesh(renderObjMesh.c_str());

		RenderObj* renderObj;
		if (mAssetManager->LoadMesh(renderObjMesh.c_str())->IsSkinned())
		{
			renderObj = new SkinnedObj(mAssetManager->LoadMesh(renderObjMesh.c_str()));
			PerObjectConstants objConst;
			objConst.modelToWorldMatrix = modelToWorldMatrix;
			renderObj->SetObjectConstants(objConst);
			renderObj->SetPosition(renderObjPosition);
		}
		else
		{
			renderObj = new RenderObj(mAssetManager->LoadMesh(renderObjMesh.c_str()));
			PerObjectConstants objConst;
			objConst.modelToWorldMatrix = modelToWorldMatrix;
			renderObj->SetObjectConstants(objConst);
			renderObj->SetPosition(renderObjPosition);
		}
		
		// Read render object components
		std::string componentType;

		if (!renderObjectsDoc[i]["components"].IsArray())
		{
			DbgAssert(false, "Level File Invalid Render Object -> Component Array");
			return false;
		}

		// There are components in this component array
		if (renderObjectsDoc[i]["components"].Empty() == false)
		{
			for (unsigned int j = 0; j < renderObjectsDoc[i]["components"].Size(); j++)
			{
				if (!renderObjectsDoc[i]["components"][j].IsObject())
				{
					DbgAssert(false, "Level File Invalid Component Object");
					return false;
				}

				if (GetStringFromJSON(renderObjectsDoc[i]["components"][j], "type", componentType) == false)
				{
					DbgAssert(false, "Level File Invalid Render Object -> Components -> Type");
					return false;
				}

				// Create component based on type
				if (componentType == "PointLight")
				{
					PointLight* pLight = new PointLight(this, renderObj);
					pLight->LoadProperties(renderObjectsDoc[i]["components"][j]);
					renderObj->AddComponent(pLight);
				}
				else if (componentType == "Character")
				{
					Character* pCharacter = new Character(this, static_cast<SkinnedObj*>(renderObj));
					pCharacter->LoadProperties(renderObjectsDoc[i]["components"][j]);
					renderObj->AddComponent(pCharacter);
				}
				else if (componentType == "Player")
				{
					SkinnedObj* skinnedObj = static_cast<SkinnedObj*>(renderObj);
					Player* pPlayer = new Player(skinnedObj, this);
					pPlayer->LoadProperties(renderObjectsDoc[i]["components"][j]);
					renderObj->AddComponent(pPlayer);
				}
				else if (componentType == "FollowCam")
				{
					FollowCam* pFollowCam = new FollowCam(renderObj, this);
					pFollowCam->LoadProperties(renderObjectsDoc[i]["components"][j]);
					renderObj->AddComponent(pFollowCam);
				}
				else if (componentType == "CollisionBox")
				{
					CollisionBox* pCollisionBox = new CollisionBox(renderObj, mPhysics);
					pCollisionBox->LoadProperties(renderObjectsDoc[i]["components"][j]);
					renderObj->AddComponent(pCollisionBox);
				}
				else if (componentType == "SimpleRotate")
				{
					SimpleRotate* pSimpleRotate = new SimpleRotate(renderObj);
					pSimpleRotate->LoadProperties(renderObjectsDoc[i]["components"][j]);
					renderObj->AddComponent(pSimpleRotate);
				}
			}
		}

		mRenderObjects.push_back(renderObj);
	}

	return true;
}

// Pool Allocation for point lights
Lights::PointLightData* Game::AllocateLight()
{
	for (int i = 0; i < Lights::MMAX_POINT_LIGHTS; i++)
	{
		if (mLightConstants.c_pointLight[i].isEnabled == false)
		{
			mLightConstants.c_pointLight[i].isEnabled = true;
			return &mLightConstants.c_pointLight[i];
		}
	}

	return nullptr;
}

void Game::FreeLight(Lights::PointLightData* pLight)
{
	pLight->isEnabled = false;
}

void Game::SetAmbientLight(const Vector3& color)
{
	mLightConstants.c_ambient = color;
}

const Vector3& Game::GetAmbientLight() const
{
	return mLightConstants.c_ambient;
}