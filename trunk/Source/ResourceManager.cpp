#include "scge\ResourceManager.h"

#include <thread>

ResourceManager::ResourceManager(FileSystem& fileSystem, Console& console)
	: mFileSystem(fileSystem)
	, mConsole(console)
	, mNumberOfThreads(console, "ResourceManager.NumThreads", "Number of threads used for loading resources", 0u, std::max(std::thread::hardware_concurrency(), 1u) - 1u, std::max(std::thread::hardware_concurrency(), 1u) - 1u)
	, mActiveWorkers(0)
	, mIsStopping(false)
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

	mFileSystem.updateChangedFiles(*this);

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
}

bool ResourceManager::isLoading() const
{
	return mActiveWorkers > 0 || !mResourcesToLoad.empty();
}

void ResourceManager::stopLoads()
{
	mIsStopping = true;
	mLoadCondition.notify_all();
	for(auto &worker : mWorkers)
		worker.join();
	mWorkers.clear();
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

void ResourceManager::loadResource(Resource &resource)
{
	resource.setResourceStatus(ResourceStatus::Loading);
	if(mNumberOfThreads.getValue() == 0u)
		resourceLoadingFunction(resource);
	else
	{
		std::unique_lock<std::mutex> lock(mLoadMutex);
		mResourcesToLoad.push(&resource);
		lock.unlock();
		mLoadCondition.notify_one();
	}
}

void ResourceManager::reloadResource(Resource &resource)
{
	if(resource.getResourceStatus() != ResourceStatus::Loading)
	{
		resource.Release();
		resource.setResourceStatus(ResourceStatus::Unloaded);
		loadResource(resource);
	}
}

void ResourceManagerWorker::operator()()
{
	while(true)
	{
		std::unique_lock<std::mutex> lock(mResourceManager.mLoadMutex);
		while(!mResourceManager.mIsStopping && mResourceManager.mResourcesToLoad.empty())
			mResourceManager.mLoadCondition.wait(lock);

		if(mResourceManager.mIsStopping)
			return;

		Resource & resource = *mResourceManager.mResourcesToLoad.front();
		++mResourceManager.mActiveWorkers;
		mResourceManager.mResourcesToLoad.pop();
		lock.unlock();

		mResourceManager.resourceLoadingFunction(resource);
		--mResourceManager.mActiveWorkers;
	}
}