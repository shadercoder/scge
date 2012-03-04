#ifndef ModelResource_h__
#define ModelResource_h__

#include "scge\FileSystem\FileResource.h"
#include "scge\Graphics\Base\MeshResource.h"

class ModelResourceData : public FileResourceData
{
public:
	ModelResourceData(FileSystem &fileSystem) : FileResourceData(fileSystem) {}

	virtual std::string getFactory() const { return "Model"; }
};

class ModelResource : public FileResource
{
public:
	ModelResource(const ModelResourceData *data) : FileResource(data) {}

	virtual unsigned int numMeshes() const = 0;

	virtual const MeshResource &getMesh(unsigned int meshNumber) const = 0;

	template <typename I>
	class Interface : public FileResource::Interface<I>
	{
	public:
		Interface(I *modelResource) : FileResource::Interface<I>(modelResource) {}

		unsigned int numMeshes() const { return getResource().numMeshes(); }

		const MeshResource &getMesh(unsigned int meshNumber) const { return getResource().getMesh(meshNumber); }
	};
};

#endif // ModelResource_h__