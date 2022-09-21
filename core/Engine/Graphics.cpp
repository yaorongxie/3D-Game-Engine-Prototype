#include "stdafx.h"
#include "Graphics.h"
#include "Shader.h"
#include "engineMath.h"

#pragma comment (lib, "d3d11.lib") 

/*static*/ Graphics* Graphics::s_theGraphics = nullptr;


Graphics::Graphics()
    : mScreenWidth(0)
    , mScreenHeight(0)
    , mSwapChain(nullptr)
    , mDev(nullptr)
    , mDevCon(nullptr)
    , mBackBuffer(nullptr)
    , mCurrentRenderTarget(nullptr)
    , mDepthStencilState(nullptr)
    , mTexture2D(nullptr)
    , mDepthStencilView(nullptr)
    , mSamplerState(nullptr)
{
    DbgAssert(nullptr == s_theGraphics, "You can only have 1 Graphics");
    s_theGraphics = this;
}

Graphics::~Graphics()
{
    DbgAssert(this == s_theGraphics, "There can only be 1 Graphics");
    s_theGraphics = nullptr;
}

void Graphics::InitD3D(HWND hWnd, float width, float height)
{
    mScreenWidth = width;
    mScreenHeight = height;

    HRESULT hr = S_OK;

    { // set up the swap chain
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = static_cast<UINT>(width);
        sd.BufferDesc.Height = static_cast<UINT>(height);
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        D3D_FEATURE_LEVEL  FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
        UINT               numLevelsRequested = 1;
        D3D_FEATURE_LEVEL  FeatureLevelsSupported;
        hr = D3D11CreateDeviceAndSwapChain(NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
#ifdef _DEBUG
            D3D11_CREATE_DEVICE_DEBUG,
#else
            0,
#endif
            & FeatureLevelsRequested,
            numLevelsRequested,
            D3D11_SDK_VERSION,
            &sd,
            &mSwapChain,
            &mDev,
            &FeatureLevelsSupported,
            &mDevCon);
        DbgAssert(hr == S_OK, "Failed to create device");
    }

    { // set up the viewport (full screen)
        SetViewport(0.0f, 0.0f, width, height);
    }

    { // grab the back-buffer as our render target
        ID3D11Texture2D* pBackBuffer;
        hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        DbgAssert(hr == S_OK, "Something wrong with your back buffer");
        hr = mDev->CreateRenderTargetView(pBackBuffer, nullptr, &mBackBuffer);
        DbgAssert(hr == S_OK, "Something went wrong with your back buffer");
        pBackBuffer->Release();
    }

    // we'll be drwaing triangle lists
    mDevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // Set up depth-stencil state
    mDepthStencilState = CreateDepthStencilState(D3D11_COMPARISON_LESS_EQUAL);
    // Bind depth-stencil state
    mDevCon->OMSetDepthStencilState(mDepthStencilState, 0);
    // Release depth-stencil state
    mDepthStencilState->Release();

    // Create depth-stencil buffer
    bool success = CreateDepthStencil(static_cast<int>(width), static_cast<int>(height), &mTexture2D, &mDepthStencilView);
    DbgAssert(success == true, "Something went wrong with your CreateDepthStencil function");

    // Create sampler state
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = mDev->CreateSamplerState(&sampDesc, &mSamplerState);
    DbgAssert(hr == S_OK, "Something went wrong with your CreateSamplerState function");
    // Set sampler state to be active
    SetActiveSampler(0, mSamplerState);
    // Release sampler state
    mSamplerState->Release();
}

void Graphics::CleanD3D()
{
#ifdef _DEBUG
    ID3D11Debug* pDbg = nullptr;
    HRESULT hr = mDev->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pDbg));
    DbgAssert(S_OK == hr, "Unable to create debug device");
#endif

    mSwapChain->Release();
    mDev->Release();
    mDevCon->Release();
    mBackBuffer->Release();
    mTexture2D->Release();
    mDepthStencilView->Release();

#ifdef _DEBUG
    pDbg->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
    pDbg->Release();
#endif
}

void Graphics::SetRenderTarget(ID3D11RenderTargetView* renderTarget, ID3D11DepthStencilView* depthStencilView)
{
    mCurrentRenderTarget = renderTarget;
    mDevCon->OMSetRenderTargets(1, &mCurrentRenderTarget, depthStencilView);
}

void Graphics::ClearRenderTarget(const Color4 &clearColor)
{
    mDevCon->ClearRenderTargetView(mCurrentRenderTarget, reinterpret_cast<const float*>(&clearColor));
}

void Graphics::EndFrame()
{
    mSwapChain->Present(1, 0);
}

ID3D11Buffer* Graphics::CreateGraphicsBuffer(const void* initialData, int inDataSize, D3D11_BIND_FLAG inBindFlags, D3D11_CPU_ACCESS_FLAG inCPUAccessFlags, D3D11_USAGE inUsage)
{
    ID3D11Buffer* buffer = nullptr;
    D3D11_BUFFER_DESC desc;
    ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
    desc.Usage = inUsage;
    desc.ByteWidth = inDataSize;
    desc.BindFlags = inBindFlags;
    desc.CPUAccessFlags = inCPUAccessFlags;
    HRESULT hr = GetDevice()->CreateBuffer(&desc, nullptr, &buffer);
    DbgAssert(hr == S_OK, "Unable to create graphics buffer");

    if (nullptr != initialData)
        UploadBuffer(buffer, initialData, inDataSize);

    return buffer;
}

void Graphics::UploadBuffer(ID3D11Buffer* buffer, const void* data, size_t dataSize)
{
    D3D11_MAPPED_SUBRESOURCE map;
    HRESULT hr = GetDeviceContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &map);
    DbgAssert(hr == S_OK, "Map failed");
    memcpy(map.pData, data, dataSize);
    GetDeviceContext()->Unmap(buffer, 0);
}

void Graphics::SetViewport(float x, float y, float width, float height)
{
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
    viewport.TopLeftX = x;
    viewport.TopLeftY = y;
    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    mDevCon->RSSetViewports(1, &viewport);
}

ID3D11DepthStencilState* Graphics::CreateDepthStencilState(D3D11_COMPARISON_FUNC inDepthComparisonFunction)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    // Disable stencil testing because we don't need it
    dsDesc.StencilEnable = false;
    // Enable depth testing 
    dsDesc.DepthEnable = true;
    // Turn on writes to the depth-stencil buffer
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    // Set comparison function for depth testing
    dsDesc.DepthFunc = inDepthComparisonFunction;

    // Create depth-stencil state
    ID3D11DepthStencilState* depthStencilState;
    mDev->CreateDepthStencilState(&dsDesc, &depthStencilState);

    return depthStencilState;
}

bool Graphics::CreateDepthStencil(int inWidth, int inHeight, ID3D11Texture2D** pDepthTexture,
    ID3D11DepthStencilView** pDepthView)
{
    HRESULT hr = S_OK;

    // Create 2D texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory(&descDepth, sizeof(descDepth));
    descDepth.Width = inWidth;
    descDepth.Height = inHeight;
    descDepth.MipLevels = 0;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = mDev->CreateTexture2D(&descDepth, NULL, pDepthTexture);

    // Check whether CreateTexture2D fails
    if (hr != S_OK)
    {
        return false;
    }

    // Create Depth-Stencil View
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(descDSV));
    descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Flags = 0;
    descDSV.Texture2D.MipSlice = 0;
    hr = mDev->CreateDepthStencilView(*pDepthTexture, &descDSV, pDepthView);

    // Check whether CreateDepthStencilView fails
    if (hr != S_OK)
    {
        return false;
    }

    return true;
}

ID3D11DepthStencilView* Graphics::getDepthStencilView()
{
    return mDepthStencilView;
}

void Graphics::ClearDepthBuffer(ID3D11DepthStencilView* depthView, float depth)
{
    mDevCon->ClearDepthStencilView(depthView, D3D11_CLEAR_DEPTH, depth, 0);
}

void Graphics::SetActiveTexture(int slot, ID3D11ShaderResourceView* pView)
{
    mDevCon->PSSetShaderResources(slot, 1, &pView);
}

void Graphics::SetActiveSampler(int slot, ID3D11SamplerState* pSampler)
{
    mDevCon->PSSetSamplers(slot, 1, &pSampler);
}

ID3D11BlendState* Graphics::CreateBlendState(bool enable, D3D11_BLEND srcBlend, D3D11_BLEND dstBlend)
{
    ID3D11BlendState* pBlendState = nullptr;

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.IndependentBlendEnable = false;
    blendDesc.RenderTarget[0].BlendEnable = enable;
    blendDesc.RenderTarget[0].SrcBlend = srcBlend;
    blendDesc.RenderTarget[0].DestBlend = dstBlend;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = srcBlend;
    blendDesc.RenderTarget[0].DestBlendAlpha = dstBlend;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    HRESULT hr = S_OK;
    hr = Graphics::Get()->GetDevice()->CreateBlendState(&blendDesc, &pBlendState);
    DbgAssert(hr == S_OK, "Fail to create blend state in Graphics::CreateBlendState!");

    return pBlendState;
}

void Graphics::SetBlendState(ID3D11BlendState* inBlendState)
{
    float blendFactor[4];
    // Setup the blend factor.
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;
    Graphics::Get()->GetDeviceContext()->OMSetBlendState(inBlendState, NULL, 0xffffffff);
}