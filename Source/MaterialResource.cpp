#include "scge\Graphics\Resources\MaterialResource.h"

#include <algorithm>
#include <fstream>
#include <string>

MaterialResourceData::MaterialResourceData(ResourceManager &resourceManager, FileSystem &fileSystem, const std::string &arguments)
	: FileResourceData(fileSystem)
	, mResourceManager(resourceManager)
{
	mFileName = arguments;
}

std::string MaterialResourceData::getIdentifier() const
{
	return mFileName;
}

std::shared_ptr<Resource> MaterialResourceData::createResource() const
{
	return std::shared_ptr<Resource>(new MaterialResource(this));
}

bool MaterialResource::Load()
{
	std::ifstream file = mResourceData->mFileSystem.OpenFileRead(mResourceData->mFileName);
	if(!file || !file.is_open())
		return true;

	int fileVersion = 0;
	file >> fileVersion;
	if(fileVersion != 1)
		return true;

	file >> mDiffuseTextureName;
	if(mDiffuseTextureName.length() == 0u)
		mDiffuseTextureName = "[Alias] BlankDiffuse";

	file >> mNormalTextureName;
	if(mNormalTextureName.length() == 0u)
		mNormalTextureName = "[Alias] BlankNormal";

	file >> mSpecularTextureName;
	if(mSpecularTextureName.length() == 0u)
		mSpecularTextureName = "[Alias] BlankSpecular";

	file >> mMaterialTypeName;
	if(mMaterialTypeName.length() == 0u)
		mMaterialTypeName = "Blank";

	return false;
}

bool MaterialResource::Finalise()
{
	mDiffuseTexture = mResourceData->mResourceManager.getResourceReference<TextureResource>(mDiffuseTextureName);
	if(!mDiffuseTexture)
		return true;
	mDiffuseTextureName.clear();

	mNormalTexture = mResourceData->mResourceManager.getResourceReference<TextureResource>(mNormalTextureName);
	if(!mNormalTexture)
		return true;
	mNormalTextureName.clear();

	mSpecularTexture = mResourceData->mResourceManager.getResourceReference<TextureResource>(mSpecularTextureName);
	if(!mSpecularTexture)
		return true;
	mSpecularTextureName.clear();

	registerForChanges();

	return false;
}

void MaterialResource::Release()
{
	unregisterForChanges();

	mDiffuseTextureName.clear();
	mDiffuseTexture.Reset();

	mNormalTextureName.clear();
	mNormalTexture.Reset();

	mSpecularTextureName.clear();
	mSpecularTexture.Reset();

	mMaterialTypeName.clear();
}

ResourceStatus MaterialResource::getResourceStatus() const
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