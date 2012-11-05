#include "scge\Graphics\DirectX11\DirectX11Texture2DHelper.h"

bool DirectX11Texture2DHelper::Initialise(ID3D11Device* d3dDevice, int width, int height, DXGI_FORMAT format, UINT bindFlags, int mipLevels)
{
	return InternalConstruct(d3dDevice, width, height, format, bindFlags, mipLevels, 1, 1, 0, D3D11_RTV_DIMENSION_TEXTURE2D, D3D11_UAV_DIMENSION_TEXTURE2D, D3D11_SRV_DIMENSION_TEXTURE2D);
}

bool DirectX11Texture2DHelper::Initialise(ID3D11Device* d3dDevice, int width, int height, DXGI_FORMAT format, const DXGI_SAMPLE_DESC& sampleDesc, UINT bindFlags)
{
	return InternalConstruct(d3dDevice, width, height, format, bindFlags, 1, 1, sampleDesc.Count, sampleDesc.Quality, D3D11_RTV_DIMENSION_TEXTURE2DMS, D3D11_UAV_DIMENSION_UNKNOWN, D3D11_SRV_DIMENSION_TEXTURE2DMS);
}

bool DirectX11Texture2DHelper::Initialise(ID3D11Device* d3dDevice, int width, int height, DXGI_FORMAT format, UINT bindFlags, int mipLevels, int arraySize)
{
	return InternalConstruct(d3dDevice, width, height, format, bindFlags, mipLevels, arraySize, 1, 0, D3D11_RTV_DIMENSION_TEXTURE2DARRAY, D3D11_UAV_DIMENSION_TEXTURE2DARRAY, D3D11_SRV_DIMENSION_TEXTURE2DARRAY);
}

bool DirectX11Texture2DHelper::Initialise(ID3D11Device* d3dDevice, int width, int height, DXGI_FORMAT format, UINT bindFlags, int arraySize, const DXGI_SAMPLE_DESC& sampleDesc)
{
	return InternalConstruct(d3dDevice, width, height, format, bindFlags, 1, arraySize, sampleDesc.Count, sampleDesc.Quality, D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY, D3D11_UAV_DIMENSION_UNKNOWN, D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY);
}

void DirectX11Texture2DHelper::Release()
{
	mShaderResourceElements.clear();
	mShaderResource.Release();

	mUnorderedAccessElements.clear();
	mRenderTargetElements.clear();

	mTexture.Release();
}

bool DirectX11Texture2DHelper::InternalConstruct(ID3D11Device* d3dDevice, int width, int height, DXGI_FORMAT format, UINT bindFlags, int mipLevels, int arraySize, int sampleCount, int sampleQuality, D3D11_RTV_DIMENSION rtvDimension, D3D11_UAV_DIMENSION uavDimension, D3D11_SRV_DIMENSION srvDimension)
{
	CD3D11_TEXTURE2D_DESC textureDesc(format, width, height, arraySize, mipLevels, bindFlags, D3D11_USAGE_DEFAULT, 0, sampleCount, sampleQuality, mipLevels != 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0);

	if(FAILED(d3dDevice->CreateTexture2D(&textureDesc, 0, mTexture.getModifiablePointer())))
		return true;

	mTexture->GetDesc(&textureDesc);

	if(bindFlags & D3D11_BIND_RENDER_TARGET)
	{
		mRenderTargetElements.resize(arraySize);
		for(int i = 0; i < arraySize; ++i)
		{
			CD3D11_RENDER_TARGET_VIEW_DESC textureRTVDesc(rtvDimension, format, 0, i, 1);
			if(FAILED(d3dDevice->CreateRenderTargetView(mTexture, &textureRTVDesc, mRenderTargetElements[i].getModifiablePointer())))
				return true;
		}
	}

	if(bindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		mUnorderedAccessElements.resize(arraySize);
		for(int i = 0; i < arraySize; ++i)
		{
			CD3D11_UNORDERED_ACCESS_VIEW_DESC textureUAVDesc(uavDimension, format, 0, i, 1);
			if(FAILED(d3dDevice->CreateUnorderedAccessView(mTexture, &textureUAVDesc, mUnorderedAccessElements[i].getModifiablePointer())))
				return true;
		}
	}

	if(bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc(srvDimension, format, 0, textureDesc.MipLevels, 0, textureDesc.ArraySize);        
		d3dDevice->CreateShaderResourceView(mTexture, &srvDesc, mShaderResource.getModifiablePointer());

		mShaderResourceElements.resize(arraySize);
		for(int i = 0; i < arraySize; ++i)
		{
			CD3D11_SHADER_RESOURCE_VIEW_DESC srvElementDesc(srvDimension, format, 0, 1, i, 1);
			if(FAILED(d3dDevice->CreateShaderResourceView(mTexture, &srvElementDesc, mShaderResourceElements[i].getModifiablePointer())))
				return true;
		}
	}

	return false;
}