#include "scge\Graphics\DirectX11\DirectX11Material.h"

std::shared_ptr<Resource> DirectX11MaterialData::createResource() const
{
	return std::shared_ptr<Resource>(new DirectX11MaterialResource(this));
}

bool DirectX11MaterialResource::Load()
{
	if(LoadMaterial(*mResourceData))
	{
		mResourceData->mConsole.threadSafePrintError(StringUtility::format("Failed to load material %", mResourceData->mFileName));
		return true;
	}

	return false;
}

bool DirectX11MaterialResource::Finalise()
{
	mDiffuseTexture = mResourceData->mResourceManager.getResourceReference<DirectX11Texture>(mDiffuseTextureName);
	if(!mDiffuseTexture)
		return true;
	mDiffuseTextureName.clear();

	mNormalTexture = mResourceData->mResourceManager.getResourceReference<DirectX11Texture>(mNormalTextureName);
	if(!mNormalTexture)
		return true;
	mNormalTextureName.clear();

	mSpecularTexture = mResourceData->mResourceManager.getResourceReference<DirectX11Texture>(mSpecularTextureName);
	if(!mSpecularTexture)
		return true;
	mSpecularTextureName.clear();

	registerForChanges();

	return false;
}

void DirectX11MaterialResource::Release()
{
	unregisterForChanges();

	mDiffuseTextureName.clear();
	mDiffuseTexture.Reset();

	mNormalTextureName.clear();
	mNormalTexture.Reset();

	mSpecularTextureName.clear();
	mSpecularTexture.Reset();
}

ResourceStatus DirectX11MaterialResource::getResourceStatus() const
{
	ResourceStatus baseStatus = Resource::getResourceStatus();
	if(baseStatus != ResourceStatus::Ready)
		return baseStatus;

	baseStatus = std::min(baseStatus, mDiffuseTexture->getResourceStatus());
	baseStatus = std::min(baseStatus, mNormalTexture->getResourceStatus());
	baseStatus = std::min(baseStatus, mSpecularTexture->getResourceStatus());

	if(baseStatus == ResourceStatus::Ready || baseStatus == ResourceStatus::FinaliseError || baseStatus == ResourceStatus::LoadError)
		return baseStatus;
	return ResourceStatus::Loading;
}