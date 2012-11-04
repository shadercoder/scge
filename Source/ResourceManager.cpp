#include "scge\ResourceManager.h"

#include <thread>

ResourceManager::ResourceManager(Console& console)
	: mConsole(console)
	, mNumberOfThreads(console, "ResourceManager.NumThreads", "Number of threads used for loading resources", 0u, std::max(std::thread::hardware_concurrency(), 1u) - 1u, std::max(std::thread::hardware_concurrency(), 1u) - 1u)
	, mIsStopping(false)
	, mIsWaiting(false)
{
	resumeLoads();
}

ResourceManager::~ResourceManager()
{
	stopLoads();

	for(const auto & indentifierToResource : mIdentifierToResources)
	{
		indentifierToResource.second->Release();
		indentifierToResource.second->setResourceStatus(ResourceStatus::Released);
	}
	mIdentifierToResources.clear();
}

void ResourceManager::GarbageCollect()
{
	releaseResourceIf([](const Resource &resource)
	{
		return resource.getReferenceCount() == 0;
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
	releaseResourceIf([&typeName](const Resource &resource)
	{
		return resource.getFactory() == typeName;
	});
}

void ResourceManager::Update()
{
	if(mNumberOfThreads.hasValueChanged())
	{
		mNumberOfThreads.updateValue();

		stopLoads();
		resumeLoads();
	}

	while(!mResourcesToFinalise.empty())
	{
		std::unique_lock<std::mutex> lock(mLoadMutex);
		Resource & resource = *mResourcesToFinalise.front();
		mResourcesToFinalise.pop();
		lock.unlock();

		if(resource.Finalise())
		{
			resource.Release();
			resource.setResourceStatus(ResourceStatus::FinaliseError);
		}
		else
			resource.setResourceStatus(ResourceStatus::Ready);
	}

	// TODO: Make all locations push onto queue so this isn't needed, will also make it safer
	for(auto & indentifierToResource : mIdentifierToResources)
	{
		switch(indentifierToResource.second->getResourceStatus())
		{
		case ResourceStatus::Unloaded:
			loadResource(indentifierToResource.second); break;
		}
	}
}

void ResourceManager::waitForLoads()
{
	mIsWaiting = true;
	mLoadCondition.notify_all();
	for(auto &worker : mWorkers)
		worker.join();

	mIsWaiting = false;
	mWorkers.reserve(mNumberOfThreads);
	for(size_t i = 0; i < mNumberOfThreads; ++i)
		mWorkers.push_back(std::thread(ResourceManagerWorker(*this)));
}

void ResourceManager::stopLoads()
{
	mIsStopping = true;
	mLoadCondition.notify_all();
	for(auto &worker : mWorkers)
		worker.join();
}

void ResourceManager::resumeLoads()
{
	mIsStopping = false;
	mWorkers.reserve(mNumberOfThreads);
	for(size_t i = 0; i < mNumberOfThreads; ++i)
		mWorkers.push_back(std::thread(ResourceManagerWorker(*this)));
}

void ResourceManager::resourceLoadingFunction(Resource &resource)
{
	if(resource.Load())
	{
		resource.Release();
		resource.setResourceStatus(ResourceStatus::LoadError);
	}
	else
	{
		resource.setResourceStatus(ResourceStatus::Loaded);
		std::unique_lock<std::mutex> lock(mLoadMutex);
		mResourcesToFinalise.push(&resource);
	}
}

void ResourceManager::loadResource(std::unique_ptr<Resource> &resource)
{
	resource->setResourceStatus(ResourceStatus::Loading);
	if(mNumberOfThreads.getValue() == 0u)
		resourceLoadingFunction(*resource);
	else
	{
		std::unique_lock<std::mutex> lock(mLoadMutex);
		mResourcesToLoad.push(resource.get());
		lock.unlock();
		mLoadCondition.notify_one();
	}
}

void ResourceManagerWorker::operator()()
{
	while(true)
	{
		std::unique_lock<std::mutex> lock(mResourceManager.mLoadMutex);
		while(!mResourceManager.mIsStopping && !mResourceManager.mIsWaiting && mResourceManager.mResourcesToLoad.empty())
			mResourceManager.mLoadCondition.wait(lock);

		if(mResourceManager.mIsStopping || mResourceManager.mResourcesToLoad.empty())
			return;

		Resource & resource = *mResourceManager.mResourcesToLoad.front();
		mResourceManager.mResourcesToLoad.pop();
		lock.unlock();

		mResourceManager.resourceLoadingFunction(resource);
	}
}