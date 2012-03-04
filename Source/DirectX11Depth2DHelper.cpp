#include "scge\Graphics\DirectX11\DirectX11Depth2DHelper.h"

bool DirectX11Depth2DHelper::Initialise(ID3D11Device* d3dDevice, int width, int height, UINT bindFlags, bool stencil)
{
	return InternalConstruct(d3dDevice, width, height, bindFlags, 1, 1, 0, D3D11_DSV_DIMENSION_TEXTURE2D, D3D11_SRV_DIMENSION_TEXTURE2D, stencil);
}

bool DirectX11Depth2DHelper::Initialise(ID3D11Device* d3dDevice, int width, int height, const DXGI_SAMPLE_DESC& sampleDesc, UINT bindFlags, bool stencil)
{
	return InternalConstruct(d3dDevice, width, height, bindFlags, 1, sampleDesc.Count, sampleDesc.Quality, D3D11_DSV_DIMENSION_TEXTURE2DMS, D3D11_SRV_DIMENSION_TEXTURE2DMS, stencil);
}

bool DirectX11Depth2DHelper::Initialise(ID3D11Device* d3dDevice, int width, int height, UINT bindFlags, int arraySize, bool stencil)
{
	return InternalConstruct(d3dDevice, width, height, bindFlags, arraySize, 1, 0, D3D11_DSV_DIMENSION_TEXTURE2DARRAY, D3D11_SRV_DIMENSION_TEXTURE2DARRAY, stencil);
}

bool DirectX11Depth2DHelper::Initialise(ID3D11Device* d3dDevice, int width, int height, UINT bindFlags, int arraySize, const DXGI_SAMPLE_DESC& sampleDesc, bool stencil)
{
	return InternalConstruct(d3dDevice, width, height, bindFlags, arraySize, sampleDesc.Count, sampleDesc.Quality, D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY, D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY, stencil);
}

void DirectX11Depth2DHelper::Release()
{
	mDepthStencilElements.clear();
	mShaderResource.Release();
	mTexture.Release();
}

bool DirectX11Depth2DHelper::InternalConstruct(ID3D11Device* d3dDevice, int width, int height, UINT bindFlags, int arraySize, int sampleCount, int sampleQuality, D3D11_DSV_DIMENSION dsvDimension, D3D11_SRV_DIMENSION srvDimension, bool stencil)
{
	CD3D11_TEXTURE2D_DESC desc(stencil ? DXGI_FORMAT_R32G8X24_TYPELESS : DXGI_FORMAT_R32_TYPELESS, width, height, arraySize, 1, bindFlags, D3D11_USAGE_DEFAULT, 0, sampleCount, sampleQuality);

	if(FAILED(d3dDevice->CreateTexture2D(&desc, 0, &mTexture)))
		return true;

	if(bindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		mDepthStencilElements.resize(arraySize);
		for(int i = 0; i < arraySize; ++i)
		{
			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc(dsvDimension, stencil ? DXGI_FORMAT_D32_FLOAT_S8X24_UINT : DXGI_FORMAT_D32_FLOAT, 0, i, 1);
			if(FAILED(d3dDevice->CreateDepthStencilView(mTexture, &depthStencilDesc, &mDepthStencilElements[i])))
				return true;
		}
	}

	if(bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		CD3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc(srvDimension, stencil ? DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS : DXGI_FORMAT_R32_FLOAT, 0, 1, 0, arraySize);
		if(FAILED(d3dDevice->CreateShaderResourceView(mTexture, &shaderResourceDesc, &mShaderResource)))
			return true;
	}

	return false;
}