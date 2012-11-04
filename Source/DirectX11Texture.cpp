#include "scge\Graphics\DirectX11\DirectX11Texture.h"

#include <string>
#include <fstream>
#include <sstream>

DirectX11TextureData::DirectX11TextureData(ID3D11Device *d3d11Device, Console &console, FileSystem &fileSystem, bool multiThreadLoad, const std::string &arguments)
	: TextureResourceData(fileSystem)
	, mConsole(console)
	, mD3D11Device(d3d11Device)
	, mMultiThreadLoad(multiThreadLoad)
{
	mFileName = arguments;
}

std::unique_ptr<Resource> DirectX11TextureData::createResource() const
{
	return std::unique_ptr<Resource>(new DirectX11Texture(this));
}

bool DirectX11Texture::Load()
{
	std::ifstream file = mResourceData->mFileSystem.OpenFileRead(mResourceData->mFileName, true);
	if(!file || !file.is_open())
		return true;

	file.seekg(0, std::ios::end);
	std::ifstream::pos_type length = file.tellg();
	file.seekg(0, std::ios::beg);

	if(length <= 0)
		return true;

	mLoadingData.resize((unsigned int)length);
	file.read(mLoadingData.data(), length);

	return mResourceData->mMultiThreadLoad && finaliseLoad();
}

bool DirectX11Texture::Finalise()
{
	if(!mResourceData->mMultiThreadLoad && finaliseLoad())
		return true;

	registerForChanges();

	return false;
}

bool DirectX11Texture::finaliseLoad()
{
	if(FAILED(D3DX11CreateShaderResourceViewFromMemory(mResourceData->mD3D11Device, mLoadingData.data(), mLoadingData.size(), nullptr, nullptr, mTextureSRV.getModifieablePointer(), nullptr)))
		return true;
	mLoadingData.clear();

	return false;
}

void DirectX11Texture::Release()
{
	unregisterForChanges();
	mTextureSRV.Release();
	mLoadingData.clear();
}