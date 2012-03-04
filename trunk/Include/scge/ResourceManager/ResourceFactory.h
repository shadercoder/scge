#ifndef __ResourceFactory_h__
#define __ResourceFactory_h__

#include "scge\ResourceManager.h"

class ResourceFactory
{
public:
	ResourceFactory(const std::string &identifier)
		: mIdentifier(identifier)
		, mInitalised(false)
		, mResourceManager(nullptr)
	{}

	ResourceFactory(ResourceManager &resourceManager, const std::string &identifier)
		: mIdentifier(identifier)
		, mInitalised(false)
		, mResourceManager(&resourceManager)
	{}

	ResourceFactory(ResourceManager &resourceManager, const std::string &identifier, const std::function<std::unique_ptr<ResourceData>(const std::string&)> &factory)
		: mIdentifier(identifier)
		, mInitalised(false)
		, mResourceManager(&resourceManager)
	{
		Initialise(factory);
	}

	~ResourceFactory()
	{
		Release();
	}

	bool isInitialised() const { return mInitalised; }

	bool Initialise(const std::function<std::unique_ptr<ResourceData>(const std::string&)> &factory)
	{
		Release();

		mInitalised = mResourceManager && !mResourceManager->addResourceFactory(mIdentifier, factory);
		return !mInitalised;
	}

	bool Initialise(ResourceManager &resourceManager, const std::function<std::unique_ptr<ResourceData>(const std::string&)> &factory)
	{
		Release();

		mResourceManager = &resourceManager;
		mInitalised = mResourceManager && !mResourceManager->addResourceFactory(mIdentifier, factory);
		return !mInitalised;
	}

	void Release()
	{
		if(mInitalised)
			mResourceManager->removeResourceFactory(mIdentifier);
		mInitalised = false;
	}

private:
	std::string mIdentifier;
	bool mInitalised;
	ResourceManager *mResourceManager;

	ResourceFactory(const ResourceFactory &);
	ResourceFactory &operator=(const ResourceFactory &);
};

#endif // __ResourceFactory_h__