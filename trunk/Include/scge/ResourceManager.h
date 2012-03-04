#ifndef __ResourceManager_h__
#define __ResourceManager_h__

#include "scge\Console.h"
#include "scge\ResourceManager\ResourceReference.h"
#include "scge\Utility.h"
#include "scge\Math.h"

#include <unordered_map>
#include <functional>
#include <set>
#include <boost\threadpool.hpp>

class ResourceManager
{
public:
	ResourceManager(Console& console);
	~ResourceManager();

	void GarbageCollect();

	void Update();

	bool addResourceFactory(const std::string &typeName, std::function<std::unique_ptr<ResourceData>(const std::string&)> factory);
	void removeResourceFactory(const std::string &typeName);

	template <typename T>
	ResourceReference<T> getResourceReference(const std::string &fullIdentifier)
	{
		auto sections = StringUtility::splitSection(fullIdentifier, '(', ')');

		return getResourceReference<T>(sections.second, sections.first);
	}

	template <typename T>
	ResourceReference<T> getResourceReference(const std::string &resourceIdentifier, const std::string &referenceIdentifier)
	{
		auto originalResource = mIdentifierToResources.find(resourceIdentifier);
		if(originalResource != mIdentifierToResources.end())
		{
			ResourceReference<T> resourceReference(originalResource->second);
			resourceReference->fromInterfaceIdentifier(referenceIdentifier);
			return resourceReference;
		}

		auto sections = StringUtility::splitSection(resourceIdentifier, '[', ']');
		if(sections.first == "Alias")
			return getResourceReferenceFromAlias<T>(sections.second);

		auto factory = mResourceFactories.find(sections.first);
		if(factory == mResourceFactories.end())
		{
			mConsole.printError("No known factory to load " + resourceIdentifier);
			return ResourceReference<T>();
		}

		std::unique_ptr<ResourceData> resourceData = factory->second(sections.second);
		if(!resourceData)
		{
			mConsole.printError("ResourceData Factory Failed for " + resourceIdentifier);
			return ResourceReference<T>();
		}

		const std::string &recreatedResourceIdentifier = resourceData->getFullIdentifier();
		auto secondResource = mIdentifierToResources.find(recreatedResourceIdentifier);
		if(secondResource != mIdentifierToResources.end())
		{
			ResourceReference<T> resourceReference(secondResource->second);
			resourceReference->fromInterfaceIdentifier(referenceIdentifier);
			return resourceReference;
		}

		std::shared_ptr<Resource> resource = resourceData->createResource();
		if(!resource)
		{
			mConsole.printError("ResourceData Failed to create " + resourceIdentifier);
			return ResourceReference<T>();
		}

		resourceData.release();
		loadResource(resource);

		if(!mIdentifierToResources.insert(std::make_pair(recreatedResourceIdentifier, resource)).second)
		{
			mConsole.printError("Failed to save " + resourceIdentifier);
			return ResourceReference<T>();
		}

		ResourceReference<T> resourceReference(resource);
		resourceReference->fromInterfaceIdentifier(referenceIdentifier);
		return resourceReference;
	}

	template <typename T>
	ResourceReference<T> getResourceReferenceFromAlias(const std::string &alias)
	{
		auto it = mReferenceAliases.find(alias);
		if(it == mReferenceAliases.end() || it->second.empty())
		{
			mConsole.printError("No known resource reference alias " + alias);
			return ResourceReference<T>();
		}
		else
			return getResourceReference<T>(it->second[Math::RangeRandom(0u, it->second.size()-1u)]);
	}

	template <typename T>
	std::vector<ResourceReference<T>> getAllResourceReferencesFromAlias(const std::string &alias)
	{
		auto it = mReferenceAliases.find(alias);
		if(it == mReferenceAliases.end() || it->second.empty())
		{
			mConsole.printError("No known resource reference alias " + alias);
			return std::vector<ResourceReference<T>>();
		}
		else
		{
			std::vector<ResourceReference<T>> resourceReferences(it->second.size());
			for(int i = 0; i < it->second.size(); ++i)
				resourceReferences[i] = getResourceReference<T>(it->second[i]);

			return resourceReferences;
		}
	}

private:
	Console& mConsole;
	ConsoleVariable<unsigned int, true> mNumberOfThreads;
	boost::threadpool::pool mThreadPool;

	std::unordered_map<std::string, std::shared_ptr<Resource>> mIdentifierToResources;
	std::unordered_map<std::string, std::vector<std::string>> mReferenceAliases;
	std::unordered_map<std::string, std::function<std::unique_ptr<ResourceData>(const std::string&)>> mResourceFactories;
	
	static void resourceLoadingFunction(std::shared_ptr<Resource> resource)
	{
		if(resource->Load())
		{
			resource->Release();
			resource->setResourceStatus(ResourceStatus::LoadError);
		}
		else
			resource->setResourceStatus(ResourceStatus::Loaded);
	}

	inline void loadResource(std::shared_ptr<Resource> resource)
	{
		resource->setResourceStatus(ResourceStatus::Loading);
		if(mNumberOfThreads.getValue() == 0u || !mThreadPool.schedule(std::bind(&ResourceManager::resourceLoadingFunction, resource)))
			resourceLoadingFunction(resource);
	}

	template <typename P>
	inline void releaseResourceIf(P predicate)
	{
		for(auto it = mIdentifierToResources.begin(), end = mIdentifierToResources.end(); it != end;)
		{
			if(predicate(it->second))
			{
				// If its not loaded, wait for the thread pool so we can safely release it
				if(it->second->getResourceStatus() == ResourceStatus::Loading)
					mThreadPool.wait();

				it->second->Release();
				it->second->setResourceStatus(ResourceStatus::Released);
				mIdentifierToResources.erase(it++);
			}
			else
				++it;
		}
	}

	ResourceManager(const ResourceManager &);
	ResourceManager &operator=(const ResourceManager &);
};

#endif // __ResourceManager_h__