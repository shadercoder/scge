#include "scge\ResourceManager.h"

template <typename T>
ResourceReference<T> ResourceManager::getResourceReference(const std::string &fullIdentifier)
{
	auto sections = StringUtility::splitSection(fullIdentifier, '(', ')');

	return getResourceReference<T>(sections.second, sections.first);
}

template <typename T>
ResourceReference<T> ResourceManager::getResourceReference(const std::string &resourceIdentifier, const std::string &referenceIdentifier)
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

	std::unique_ptr<Resource> resource = resourceData->createResource();
	if(!resource)
	{
		mConsole.printError("ResourceData Failed to create " + resourceIdentifier);
		return ResourceReference<T>();
	}

	resourceData.release();
	loadResource(resource);

	auto it = mIdentifierToResources.insert(std::make_pair(recreatedResourceIdentifier, std::move(resource)));

	if(!it.second)
	{
		mConsole.printError("Failed to save " + resourceIdentifier);
		return ResourceReference<T>();
	}

	ResourceReference<T> resourceReference(it.first->second);
	resourceReference->fromInterfaceIdentifier(referenceIdentifier);
	return resourceReference;
}

template <typename T>
ResourceReference<T> ResourceManager::getResourceReferenceFromAlias(const std::string &alias)
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
std::vector<ResourceReference<T>> ResourceManager::getAllResourceReferencesFromAlias(const std::string &alias)
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

template <typename P>
void ResourceManager::releaseResourceIf(P predicate)
{
	for(auto it = mIdentifierToResources.begin(), end = mIdentifierToResources.end(); it != end;)
	{
		if(predicate(*it->second))
		{
			// If its not loaded, wait for the thread pool so we can safely release it
			if(it->second->getResourceStatus() == ResourceStatus::Loading)
				waitForLoads();

			it->second->Release();
			it->second->setResourceStatus(ResourceStatus::Released);

			if(it->second->getReferenceCount() == 0)
				mIdentifierToResources.erase(it++);
			else
				++it;
		}
		else
			++it;
	}
}