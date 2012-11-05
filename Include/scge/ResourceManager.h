#ifndef __ResourceManager_h__
#define __ResourceManager_h__

#include "scge\Console.h"
#include "scge\FileSystem.h"
#include "scge\ResourceManager\ResourceReference.h"
#include "scge\Utility.h"
#include "scge\Math.h"

#include <unordered_map>
#include <functional>
#include <set>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

class ResourceManager;
class ResourceManagerWorker
{
public:
    ResourceManagerWorker(ResourceManager &resourceManager)
		: mResourceManager(resourceManager)
	{
	}

    void operator()();

private:
    ResourceManager &mResourceManager;
};

class ResourceManager
{
public:
	ResourceManager(FileSystem& fileSystem, Console& console);
	~ResourceManager();

	void GarbageCollect();

	void Update();

	bool isLoading() const;

	bool addResourceFactory(const std::string &typeName, std::function<std::unique_ptr<ResourceData>(const std::string&)> factory);
	void removeResourceFactory(const std::string &typeName);

	template <typename T>
	ResourceReference<T> getResourceReference(const std::string &fullIdentifier);

	template <typename T>
	ResourceReference<T> getResourceReference(const std::string &resourceIdentifier, const std::string &referenceIdentifier);

	template <typename T>
	ResourceReference<T> getResourceReferenceFromAlias(const std::string &alias);

	template <typename T>
	std::vector<ResourceReference<T>> getAllResourceReferencesFromAlias(const std::string &alias);

	void reloadResource(Resource &resource);

private:
	friend class ResourceManagerWorker;

	FileSystem& mFileSystem;
	Console& mConsole;
	ConsoleVariable<unsigned int, true> mNumberOfThreads;

	std::unordered_map<std::string, std::unique_ptr<Resource>> mIdentifierToResources;
	std::unordered_map<std::string, std::vector<std::string>> mReferenceAliases;
	std::unordered_map<std::string, std::function<std::unique_ptr<ResourceData>(const std::string&)>> mResourceFactories;

	std::atomic<unsigned int> mActiveWorkers;
	std::vector<std::thread> mWorkers;
	std::queue<Resource*> mResourcesToLoad;
	std::queue<Resource*> mResourcesToFinalise;
	std::mutex mLoadMutex;
	std::condition_variable mLoadCondition;
	std::mutex mFinaliseMutex;
	bool mIsStopping;

	void stopLoads();
	void resumeLoads();
	void resourceLoadingFunction(Resource &resource);
	void loadResource(Resource &resource);

	template <typename P>
	inline void releaseResourceIf(P predicate);

	ResourceManager(const ResourceManager &);
	ResourceManager &operator=(const ResourceManager &);
};

#include "scge\ResourceManager\ResourceManager.inl"

#endif // __ResourceManager_h__