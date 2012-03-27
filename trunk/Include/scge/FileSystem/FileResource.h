#ifndef __FileResource_h__
#define __FileResource_h__

#include "scge\ResourceManager\Resource.h"
#include "scge\FileSystem.h"

class FileResourceData : public ResourceData
{
public:
	FileResourceData(FileSystem &fileSystem) : mFileSystem(fileSystem), mFileName() {}

	virtual std::string getIdentifier() const override { return mFileName; }

	std::string mFileName;

	FileSystem &mFileSystem;
};

class FileResource : public Resource
{
public:
	FileResource(const FileResourceData *fileResourceData) : Resource(fileResourceData), mFileResourceData(fileResourceData), mRegisteredForChanges(false) {}
	virtual ~FileResource() { unregisterForChanges(); }

	template <typename I>
	class Interface : public ResourceInterface<I>
	{
	public:
		Interface(I *fileResource) : ResourceInterface<I>(fileResource) {}

		const std::string &getFileName() const { return getResource().mResourceData.mFileName; }
	};

protected:
	bool registerForChanges()
	{
		if(mRegisteredForChanges)
			return true;

		mRegisteredForChanges = !mFileResourceData->mFileSystem.RegisterForFileChanges(*this, mFileResourceData->mFileName);
		return mRegisteredForChanges;
	}

	void unregisterForChanges()
	{
		if(mRegisteredForChanges)
			mFileResourceData->mFileSystem.UnregisterForFileChanges(*this);
		mRegisteredForChanges = false;
	}

private:
	bool mRegisteredForChanges;

	const FileResourceData *mFileResourceData;
};

#endif // __FileResource_h__
