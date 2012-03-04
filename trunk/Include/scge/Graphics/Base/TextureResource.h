#ifndef TextureResource_h__
#define TextureResource_h__

#include "scge\FileSystem\FileResource.h"

class TextureResourceData : public FileResourceData
{
public:
	TextureResourceData(FileSystem &fileSystem) : FileResourceData(fileSystem) {}

	virtual std::string getFactory() const { return "Texture"; }
};

class TextureResource : public FileResource
{
public:
	TextureResource(const TextureResourceData *data) : FileResource(data) {}

	template <typename I>
	class Interface : public FileResource::Interface<I>
	{
	public:
		Interface(I *textureResource) : FileResource::Interface<I>(textureResource) {}
	};
};

#endif // TextureResource_h__