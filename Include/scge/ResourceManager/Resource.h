#ifndef __Resource_h__
#define __Resource_h__

#include <string>
#include <memory>
#include <atomic>

enum class ResourceStatus
{
	Released,
	Unloaded,
	Loading,
	LoadError,
	Loaded,
	FinaliseError,
	Ready
};

template <typename T>
class ResourceInterface
{
public:
	ResourceInterface(T *resource) : mResource(resource) {}
	virtual ~ResourceInterface() {}

	virtual void fromInterfaceIdentifier(const std::string &) {}
	virtual std::string toInterfaceIdentifier() const { return ""; }

	std::string toResourceIdentifier() const { return mResource->getFullIdentifier(); }

	std::string toFullIdentifier() const { return toResourceIdentifier() + " (" + toInterfaceIdentifier() + ")"; }

	ResourceStatus getResourceStatus() const { return mResource->getResourceStatus(); }
	int getReferenceCount() const { return mResource->getReferenceCount(); }

protected:
	const T &getResource() const { return *mResource; }

private:
	T *mResource;
};

class Resource;
class ResourceData
{
public:
	virtual ~ResourceData() {}

	std::string getFullIdentifier() const { return "[" + getFactory() + "] " + getIdentifier(); }
	virtual std::string getIdentifier() const = 0;
	virtual std::string getFactory() const = 0;

	virtual std::shared_ptr<Resource> createResource() const = 0;
};

class Resource
{
public:
	Resource(const ResourceData *) : mResourceStatus(ResourceStatus::Unloaded), mReferences(0) {}
	Resource(const Resource &) = delete;
	Resource &operator=(const Resource &) = delete;
	virtual ~Resource() {}

	virtual bool Load() = 0;
	virtual bool Finalise() = 0;
	virtual void Release() = 0;

	virtual std::string getFullIdentifier() const = 0;
	virtual std::string getIdentifier() const = 0;
	virtual std::string getFactory() const = 0;

	void setResourceStatus(ResourceStatus resourceStatus) { mResourceStatus = resourceStatus; }
	virtual ResourceStatus getResourceStatus() const { return mResourceStatus; }

	unsigned int getReferenceCount() const { return mReferences; }
	void AddReference() { ++mReferences; }
	void RemoveReference() { --mReferences; }

	typedef ResourceInterface<Resource> Interface;

private:
	ResourceStatus mResourceStatus;

	std::atomic<unsigned int> mReferences;
};

template <typename DataType, typename BaseType>
class ResourceBase : public BaseType
{
public:
	ResourceBase(const DataType *data) : BaseType(data), mResourceData(data) {}

	virtual std::string getFullIdentifier() const final { return mResourceData->getFullIdentifier(); }
	virtual std::string getIdentifier() const final { return mResourceData->getIdentifier(); }
	virtual std::string getFactory() const final { return mResourceData->getFactory(); }

protected:
	const std::unique_ptr<const DataType> mResourceData;
};

#endif // __Resource_h__
