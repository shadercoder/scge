#ifndef __DirectX11Material_h__
#define __DirectX11Material_h__

#include "scge\Graphics\Resources\MaterialResource.h"

#include "scge\Graphics\DirectX11\DirectX11Texture.h"

class DirectX11MaterialData : public MaterialResourceData
{
public:
	DirectX11MaterialData(Console &console, ResourceManager &resourceManager, FileSystem &fileSystem, const std::string &arguments) : MaterialResourceData(console, resourceManager, fileSystem, arguments) { }

	virtual std::shared_ptr<Resource> createResource() const final;
};

class DirectX11MaterialResource : public ResourceBase<DirectX11MaterialData, MaterialResource>
{
public:
	DirectX11MaterialResource(const DirectX11MaterialData *data) : ResourceBase(data) {}

	virtual bool Load() final;
	virtual bool Finalise() final;
	virtual void Release() final;

	virtual ResourceStatus getResourceStatus() const final;

	virtual ResourceReference<TextureResource> getDiffuseReference() const final { return mDiffuseTexture; }
	virtual ResourceReference<TextureResource> getNormalReference() const final { return mNormalTexture; }
	virtual ResourceReference<TextureResource> getSpecularReference() const final { return mSpecularTexture; }

	const ResourceReference<DirectX11Texture> &getRawDiffuseReference() const { return mDiffuseTexture; }
	const ResourceReference<DirectX11Texture> &getRawNormalReference() const { return mNormalTexture; }
	const ResourceReference<DirectX11Texture> &getRawSpecularReference() const { return mSpecularTexture; }

	template <typename I>
	class Interface : public MaterialResource::Interface<I>
	{
	public:
		Interface(I *directX11MaterialResource) : MaterialResource::Interface<I>(directX11MaterialResource) {}

		const ResourceReference<DirectX11Texture> &getRawDiffuseReference() const { return getResource().getRawDiffuseReference(); }
		const ResourceReference<DirectX11Texture> &getRawNormalReference() const { return getResource().getRawNormalReference(); }
		const ResourceReference<DirectX11Texture> &getRawSpecularReference() const { return getResource().getRawSpecularReference(); }
	};

private:
	ResourceReference<DirectX11Texture> mDiffuseTexture;
	ResourceReference<DirectX11Texture> mNormalTexture;
	ResourceReference<DirectX11Texture> mSpecularTexture;
};

#endif // __DirectX11Material_h__