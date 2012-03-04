#ifndef DirectX11Depth2DHelper_h__
#define DirectX11Depth2DHelper_h__

#include <d3d11.h>
#include <atlbase.h>
#include <vector>

class DirectX11Depth2DHelper
{
public:
	DirectX11Depth2DHelper() {}
	~DirectX11Depth2DHelper() { Release(); }

	DirectX11Depth2DHelper(DirectX11Depth2DHelper &&other)
	{
		operator=(std::move(other));
	}

	DirectX11Depth2DHelper& operator=(DirectX11Depth2DHelper &&other)
	{
		if(this != &other)
		{
			std::swap(mTexture, other.mTexture);
			std::swap(mShaderResource, other.mShaderResource);
			std::swap(mDepthStencilElements, other.mDepthStencilElements);
		}

		return *this;
	}

	bool Initialise(ID3D11Device* d3dDevice, int width, int height, UINT bindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, bool stencil = false);
	bool Initialise(ID3D11Device* d3dDevice, int width, int height, const DXGI_SAMPLE_DESC& sampleDesc, UINT bindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, bool stencil = false);
	bool Initialise(ID3D11Device* d3dDevice, int width, int height, UINT bindFlags, int arraySize, bool stencil = false);
	bool Initialise(ID3D11Device* d3dDevice, int width, int height, UINT bindFlags, int arraySize, const DXGI_SAMPLE_DESC& sampleDesc, bool stencil = false);

	void Release();

	ID3D11Texture2D *GetTexture() const { return mTexture; }
	ID3D11Texture2D *const *GetTexturePointer() const { return &mTexture.p; }

	ID3D11DepthStencilView *GetDepthStencil(std::size_t arrayIndex = 0) const { return mDepthStencilElements[arrayIndex]; }
	ID3D11DepthStencilView *const *GetDepthStencilPointer(std::size_t arrayIndex = 0) const { return &mDepthStencilElements[arrayIndex].p; }

	ID3D11ShaderResourceView *GetShaderResource() const { return mShaderResource; }
	ID3D11ShaderResourceView *const *GetShaderResourcePointer() const { return &mShaderResource.p; }

private:
	bool InternalConstruct(ID3D11Device* d3dDevice, int width, int height, UINT bindFlags, int arraySize, int sampleCount, int sampleQuality, D3D11_DSV_DIMENSION dsvDimension, D3D11_SRV_DIMENSION srvDimension, bool stencil);

	CComPtr<ID3D11Texture2D> mTexture;
	CComPtr<ID3D11ShaderResourceView> mShaderResource;
	std::vector<CComPtr<ID3D11DepthStencilView>> mDepthStencilElements;

	DirectX11Depth2DHelper(const DirectX11Depth2DHelper&);
	DirectX11Depth2DHelper& operator=(const DirectX11Depth2DHelper&);
};

#endif // DirectX11Depth2DHelper_h__