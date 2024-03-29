#include "scge\Graphics\DirectX11\DirectX11Model.h"

#include "scge\Graphics\DirectX11\DirectX11Renderer3D.h"

#include <fstream>
#include <algorithm>

DirectX11ModelData::DirectX11ModelData(ID3D11Device *d3d11Device, DirectX11Renderer3D &directX11Renderer3D, Console &console, ResourceManager &resourceManager, FileSystem &fileSystem, bool multiThreadLoad, const std::string &arguments)
	: ModelResourceData(fileSystem)
	, mConsole(console)
	, mD3D11Device(d3d11Device)
	, mDirectX11Renderer3D(directX11Renderer3D)
	, mResourceManager(resourceManager)
	, mMultiThreadLoad(multiThreadLoad)
{
	mFileName = arguments;
}

std::unique_ptr<Resource> DirectX11ModelData::createResource() const
{
	return std::unique_ptr<Resource>(new DirectX11Model(this));
}

template <typename T>
inline bool read(std::ifstream &file, T &result)
{
	file.read((char *)&result, sizeof(T));
	return file.fail();
}

bool DirectX11Model::Load()
{
	std::ifstream file = mResourceData->mFileSystem.OpenFileRead(mResourceData->mFileName, true);
	if(!file || !file.is_open())
	{
		mResourceData->mConsole.threadSafePrintError(StringUtility::format("Failed to locate file %", mResourceData->mFileName));
		return true;
	}

	uint32 fileVersion = 0;
	if(read(file, fileVersion) || fileVersion != 1)
	{
		mResourceData->mConsole.threadSafePrintError(StringUtility::format("Unsupported file format for %", mResourceData->mFileName));
		return true;
	}

	uint32 numberOfMeshes = 0;
	if(read(file, numberOfMeshes))
		return true;

	while(numberOfMeshes > 0)
	{
		std::unique_ptr<DirectX11Mesh> mesh(new DirectX11Mesh(mResourceData->mD3D11Device, mResourceData->mConsole, mResourceData->mResourceManager, mResourceData->mFileSystem));
		if(mesh->Load(file))
			return true;

		if(mResourceData->mMultiThreadLoad && mesh->finaliseLoad())
			return true;

		mMeshs.push_back(std::move(mesh));
		--numberOfMeshes;
	}

	return false;
}

bool DirectX11Model::Finalise()
{
	for(auto & mesh : mMeshs)
	{
		if(mesh->Finalise())
			return true;

		if(!mResourceData->mMultiThreadLoad && mesh->finaliseLoad())
			return true;
	}

	registerForChanges();

	return false;
}

void DirectX11Model::Release()
{
	unregisterForChanges();
	for(auto mesh = mMeshs.begin(); mesh != mMeshs.end(); ++mesh)
		(*mesh)->Release();
	mMeshs.clear();
}

void DirectX11Model::Render() const
{
	mResourceData->mDirectX11Renderer3D.addModelToQueue(this);
}

void DirectX11Model::Render(ID3D11DeviceContext* deviceContext, bool alphaTestPass) const
{
	for(auto & mesh : mMeshs)
		mesh->Render(deviceContext, alphaTestPass);
}

ResourceStatus DirectX11Model::getResourceStatus() const
{
	ResourceStatus baseStatus = Resource::getResourceStatus();
	if(baseStatus != ResourceStatus::Ready)
		return baseStatus;

	for(auto & mesh : mMeshs)
		baseStatus = std::min(baseStatus, mesh->getResourceStatus());

	if(baseStatus == ResourceStatus::Ready || baseStatus == ResourceStatus::FinaliseError || baseStatus == ResourceStatus::LoadError)
		return baseStatus;
	return ResourceStatus::Loading;
}