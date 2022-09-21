#include "stdafx.h"
#include "texture.h"
#include "Graphics.h"
#include "DirectXTK\Inc\DDSTextureLoader.h"
#include "DirectXTK\Inc\WICTextureLoader.h"
#include <string>

Texture::Texture()
    : mResource(nullptr)
    , mView(nullptr)
    , mWidth(0)
    , mHeight(0)
{
}

Texture::~Texture()
{
    Free();
}

void Texture::Free()
{
    if (nullptr != mView)
    {
        mView->Release();
        mView = nullptr;
    }
    if (nullptr != mResource)
    {
        mResource->Release();
        mResource = nullptr;
    }

    mWidth = 0;
    mHeight = 0;
}

bool Texture::Load(const WCHAR* fileName)
{
    Free();     // in case there was already a texture loaded here, release it

    ID3D11Device *pDev = Graphics::Get()->GetDevice();

    std::wstring fileStr(fileName);
    std::wstring extension = fileStr.substr(fileStr.find_last_of('.'));
    HRESULT hr = E_FAIL;
    if (extension == L".dds" || extension == L".DDS")
        hr = DirectX::CreateDDSTextureFromFile(pDev, fileName, &mResource, &mView);
    else
        hr = DirectX::CreateWICTextureFromFile(pDev, fileName, &mResource, &mView);
    DbgAssert(hr == S_OK, "Problem Creating Texture From File");
    if (S_OK != hr)
        return false;

    CD3D11_TEXTURE2D_DESC textureDesc;
    ((ID3D11Texture2D*)mResource)->GetDesc(&textureDesc);
    mWidth = textureDesc.Width;
    mHeight = textureDesc.Height;

    return true;
}

Texture* Texture::StaticLoad(const WCHAR* fileName, AssetManager* pManager)
{
    Texture* pTex = new Texture();
    if (false == pTex->Load(fileName))
    {
        delete pTex;
        return nullptr;
    }
    return pTex;
}

void Texture::SetActive(int slot) const
{
    Graphics::Get()->SetActiveTexture(slot, mView);
}

ID3D11RenderTargetView* Texture::CreateRenderTarget(int inWidth, int inHeight, DXGI_FORMAT format)
{
    mWidth = inWidth;
    mHeight = inHeight;
    HRESULT hr = S_OK;
    ID3D11Device* pDevice = Graphics::Get()->GetDevice();


    D3D11_TEXTURE2D_DESC pTexture2DDesc;
    ZeroMemory(&pTexture2DDesc, sizeof(pTexture2DDesc));
    pTexture2DDesc.Width = inWidth;
    pTexture2DDesc.Height = inHeight;
    pTexture2DDesc.Format = format;
    pTexture2DDesc.MipLevels = 1;
    pTexture2DDesc.ArraySize = 1;
    pTexture2DDesc.SampleDesc.Count = 1;
    pTexture2DDesc.SampleDesc.Quality = 0;
    pTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
    pTexture2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    hr = pDevice->CreateTexture2D(&pTexture2DDesc, NULL, (ID3D11Texture2D**)(&mResource));
    DbgAssert(hr == S_OK, "Fail to create Texture 2D in Texture::CreateRenderTarget");


    hr = pDevice->CreateShaderResourceView((ID3D11Texture2D*)mResource, NULL, &mView);
    DbgAssert(hr == S_OK, "Fail to create Shader Resource View in Texture::CreateRenderTarget");


    ID3D11RenderTargetView* pRenderTargetView;
    hr = pDevice->CreateRenderTargetView(mResource, NULL, &pRenderTargetView);
    DbgAssert(hr == S_OK, "Fail to create Render Target View in Texture::CreateRenderTarget");
    

    return pRenderTargetView;
}
