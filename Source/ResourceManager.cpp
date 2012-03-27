#include "scge\ResourceManager.h"

#include <thread>

ResourceManager::ResourceManager(Console& console)
	: mConsole(console)
	, mNumberOfThreads(console, "ResourceManager.NumThreads", "Number of threads used for loading resources", 0u, std::max(std::thread::hardware_concurrency(), 1u) - 1u, std::max(std::thread::hardware_concurrency(), 1u) - 1u)
	, mThreadPool(mNumberOfThreads.getValue())
{
}

ResourceManager::~ResourceManager()
{
	mThreadPool.clear();
	mThreadPool.wait();
	for(const auto & indentifierToResource : mIdentifierToResources)
	{
		indentifierToResource.second->Release();
		indentifierToResource.second->setResourceStatus(ResourceStatus::Released);
	}
	mIdentifierToResources.clear();
}

void ResourceManager::GarbageCollect()
{
	releaseResourceIf([this](const std::shared_ptr<Resource> &resource)
	{
		return resource->getReferenceCount() == 0;
	});
}

bool ResourceManager::addResourceFactory(const std::string &typeName, std::function<std::unique_ptr<ResourceData>(const std::string&)> factory)
{
	if(typeName.empty() || !factory)
		return true;

	return !mResourceFactories.insert(std::make_pair(typeName, factory)).second;
}

void ResourceManager::removeResourceFactory(const std::string &typeName)
{
	mResourceFactories.erase(typeName);
	releaseResourceIf([this, &typeName](const std::shared_ptr<Resource> &resource)
	{
		return resource->getFactory() == typeName;
	});
}

void ResourceManager::Update()
{
	if(mNumberOfThreads.hasValueChanged())
	{
		mNumberOfThreads.updateValue();

		if(mNumberOfThreads > 0u)
			mThreadPool.size_controller().resize(mNumberOfThreads);
	}

	for(const auto & indentifierToResource : mIdentifierToResources)
	{
		switch(indentifierToResource.second->getResourceStatus())
		{
		case ResourceStatus::Loaded:
			if(indentifierToResource.second->Finalise())
			{
				indentifierToResource.second->Release();
				indentifierToResource.second->setResourceStatus(ResourceStatus::FinaliseError);
			}
			else
				indentifierToResource.second->setResourceStatus(ResourceStatus::Ready);
			break;

		case ResourceStatus::Unloaded:
			loadResource(indentifierToResource.second); break;
		}
	}
}