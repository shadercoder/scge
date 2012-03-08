#ifndef DirectX11Texture2DHelper_h__
#define DirectX11Texture2DHelper_h__

#include "scge\Graphics\DirectX11\DirectX11ComPtr.h"

#include <d3d11.h>
#include <vector>

class DirectX11Texture2DHelper
{
public:
	DirectX11Texture2DHelper() {}
	~DirectX11Texture2DHelper() { Release(); }

	DirectX11Texture2DHelper(DirectX11Texture2DHelper &&other)
	{
		operator=(std::move(other));
	}

	DirectX11Texture2DHelper& operator=(DirectX11Texture2DHelper &&other)
	{
		if(this != &other)
		{
			std::swap(mTexture, other.mTexture);
			std::swap(mShaderResource, other.mShaderResource);
			std::swap(mRenderTargetElements, other.mRenderTargetElements);
			std::swap(mUnorderedAccessElements, other.mUnorderedAccessElements);
			std::swap(mShaderResourceElements, other.mShaderResourceElements);
		}

		return *this;
	}

	bool Initialise(ID3D11Device* d3dDevice, int width, int height, DXGI_FORMAT format, UINT bindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, int mipLevels = 1);
	bool Initialise(ID3D11Device* d3dDevice, int width, int height, DXGI_FORMAT format, const DXGI_SAMPLE_DESC& sampleDesc, UINT bindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	bool Initialise(ID3D11Device* d3dDevice, int width, int height, DXGI_FORMAT format, UINT bindFlags, int mipLevels, int arraySize);
	bool Initialise(ID3D11Device* d3dDevice, int width, int height, DXGI_FORMAT format, UINT bindFlags, int arraySize, const DXGI_SAMPLE_DESC& sampleDesc);

	void Release();

	ID3D11Texture2D *GetTexture() const { return mTexture; }
	ID3D11Texture2D *const *GetTexturePointer() const { return &mTexture; }

	ID3D11RenderTargetView *GetRenderTarget(std::size_t arrayIndex = 0) const { return mRenderTargetElements[arrayIndex]; }
	ID3D11RenderTargetView *const *GetRenderTargetPointer(std::size_t arrayIndex = 0) const { return &mRenderTargetElements[arrayIndex]; }

	ID3D11UnorderedAccessView *GetUnorderedAccess(std::size_t arrayIndex = 0) const { return mUnorderedAccessElements[arrayIndex]; }
	ID3D11UnorderedAccessView *const *GetUnorderedAccessPointer(std::size_t arrayIndex = 0) const { return &mUnorderedAccessElements[arrayIndex]; }

	ID3D11ShaderResourceView *GetShaderResource() const { return mShaderResource; }
	ID3D11ShaderResourceView *const *GetShaderResourcePointer() const { return &mShaderResource; }

	ID3D11ShaderResourceView *GetShaderResource(std::size_t arrayIndex) const { return mShaderResourceElements[arrayIndex]; }
	ID3D11ShaderResourceView *const *GetShaderResourcePointer(std::size_t arrayIndex) const { return &mShaderResourceElements[arrayIndex]; }

private:
	bool InternalConstruct(ID3D11Device* d3dDevice,	int width, int height, DXGI_FORMAT format, UINT bindFlags, int mipLevels, int arraySize, int sampleCount, int sampleQuality, D3D11_RTV_DIMENSION rtvDimension, D3D11_UAV_DIMENSION uavDimension, D3D11_SRV_DIMENSION srvDimension);
	
	ComPtr<ID3D11Texture2D> mTexture;
	ComPtr<ID3D11ShaderResourceView> mShaderResource;

	std::vector<ComPtr<ID3D11RenderTargetView>> mRenderTargetElements;
	std::vector<ComPtr<ID3D11UnorderedAccessView>> mUnorderedAccessElements;
	std::vector<ComPtr<ID3D11ShaderResourceView>> mShaderResourceElements;

	DirectX11Texture2DHelper(const DirectX11Texture2DHelper&);
	DirectX11Texture2DHelper& operator=(const DirectX11Texture2DHelper&);
};

#endif // DirectX11Texture2DHelper_h__
