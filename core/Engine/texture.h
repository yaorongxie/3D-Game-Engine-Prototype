#pragma once

class AssetManager;

class Texture
{
    public:
        Texture();
        ~Texture();
    
        void Free();
        bool Load(const WCHAR* fileName);
        static Texture* StaticLoad(const WCHAR* fileName, AssetManager* pManager);
        int GetWidth() const { return mWidth; }
        int GetHeight() const { return mHeight; }
        void SetActive(int slot) const;
        ID3D11RenderTargetView* CreateRenderTarget(int inWidth, int inHeight, DXGI_FORMAT format);

    private:
        ID3D11Resource* mResource;
        ID3D11ShaderResourceView* mView;
        int mWidth;
        int mHeight;
};