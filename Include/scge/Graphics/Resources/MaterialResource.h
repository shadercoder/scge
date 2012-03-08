#ifndef MaterialResource_h__
#define MaterialResource_h__

#include "scge\Graphics\Base\TextureResource.h"
#include "scge\ResourceManager\ResourceReference.h"
#include "scge\ResourceManager.h"

class MaterialResourceData : public FileResourceData
{
public:
	MaterialResourceData(ResourceManager &resourceManager, FileSystem &fileSystem, const std::string &arguments);

	virtual std::string getIdentifier() const final;
	virtual std::string getFactory() const final { return "Material"; }

	virtual std::shared_ptr<Resource> createResource() const final;

	ResourceManager &mResourceManager;
};

class MaterialResource : public ResourceBase<MaterialResourceData, FileResource>
{
public:
	MaterialResource(const MaterialResourceData *data) : ResourceBase(data) {}

	virtual bool Load() final;
	virtual bool Finalise() final;
	virtual void Release() final;

	virtual ResourceStatus getResourceStatus() const final;

	ResourceReference<TextureResource> getDiffuseReference() const { return mDiffuseTexture; }
	ResourceReference<TextureResource> getNormalReference() const { return mNormalTexture; }
	ResourceReference<TextureResource> getSpecularReference() const { return mSpecularTexture; }

	template <typename I>
	class Interface : public FileResource::Interface<I>
	{
	public:
		Interface(I *fileResource) : FileResource::Interface<I>(fileResource) {}

		ResourceReference<TextureResource> getDiffuseReference() const { return getResource().getDiffuseReference(); }
		ResourceReference<TextureResource> getNormalReference() const { return getResource().getNormalReference(); }
		ResourceReference<TextureResource> getSpecularReference() const { return getResource().getSpecularReference(); }
	};

private:
	std::string mDiffuseTextureName;
	ResourceReference<TextureResource> mDiffuseTexture;

	std::string mNormalTextureName;
	ResourceReference<TextureResource> mNormalTexture;

	std::string mSpecularTextureName;
	ResourceReference<TextureResource> mSpecularTexture;

	std::string mMaterialTypeName;
};

#endif // MaterialResource_h__