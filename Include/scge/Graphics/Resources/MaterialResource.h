#ifndef MaterialResource_h__
#define MaterialResource_h__

#include "scge\Graphics\Base\TextureResource.h"
#include "scge\ResourceManager\ResourceReference.h"
#include "scge\ResourceManager.h"
#include "scge\Console.h"

enum class MaterialType
{
	Default = 0,
};

class MaterialResourceData : public FileResourceData
{
public:
	MaterialResourceData(Console &console, ResourceManager &resourceManager, FileSystem &fileSystem, const std::string &arguments);

	virtual std::string getIdentifier() const final;
	virtual std::string getFactory() const final { return "Material"; }

	Console &mConsole;
	ResourceManager &mResourceManager;
};

class MaterialResource : public FileResource
{
public:
	MaterialResource(const MaterialResourceData *data) : FileResource(data) {}

	bool LoadMaterial(const MaterialResourceData &resourceData);

	virtual ResourceReference<TextureResource> getDiffuseReference() const = 0;
	virtual ResourceReference<TextureResource> getNormalReference() const = 0;
	virtual ResourceReference<TextureResource> getSpecularReference() const = 0;

	bool isAlphaTestMaterial() const { return mIsAlphaTestMaterial; }

	template <typename I>
	class Interface : public FileResource::Interface<I>
	{
	public:
		Interface(I *materialResource) : FileResource::Interface<I>(materialResource) {}

		ResourceReference<TextureResource> getDiffuseReference() const { return getResource().getDiffuseReference(); }
		ResourceReference<TextureResource> getNormalReference() const { return getResource().getNormalReference(); }
		ResourceReference<TextureResource> getSpecularReference() const { return getResource().getSpecularReference(); }

		bool isAlphaTestMaterial() const { return getResource().isAlphaTestMaterial(); }
	};

protected:
	std::string mDiffuseTextureName;
	std::string mNormalTextureName;
	std::string mSpecularTextureName;

	MaterialType mMaterialType;

	bool mIsAlphaTestMaterial;
};

#endif // MaterialResource_h__