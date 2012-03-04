#ifndef __ResourceReference_h__
#define __ResourceReference_h__

#include "scge\ResourceManager\Resource.h"

template <typename T>
class ResourceReference
{
public:
	static_assert(std::is_convertible<T, Resource>::value, "ResourceReference can only point to a Resource Type");
	ResourceReference()
		: mResource(nullptr)
		, mInterface(nullptr)
	{}

	template <typename O>
	ResourceReference(const std::shared_ptr<O> &resource)
		: mResource(std::dynamic_pointer_cast<T>(resource))
		, mInterface(mResource.get())
	{
		static_assert(std::is_convertible<O, Resource>::value, "ResourceReference can only point to a Resource Type");
		AddReference();
	}

	template <typename O>
	ResourceReference(const ResourceReference<O> &other)
		: mResource(std::dynamic_pointer_cast<T>(other.mResource))
		, mInterface(mResource.get())
	{
		AddReference();
	}

	ResourceReference(const ResourceReference<T> &other)
		: mResource(other.mResource)
		, mInterface(other.mInterface)
	{
		AddReference();
	}

	template <typename O>
	ResourceReference &operator=(const ResourceReference<O> &other)
	{
		if((void*)this == (void*)&other)
			return *this;

		RemoveReference();
		mResource = std::dynamic_pointer_cast<T>(other.mResource);
		mInterface = T::Interface(mResource.get());
		AddReference();

		return *this;
	}

	ResourceReference &operator=(const ResourceReference<T> &other)
	{
		if(this == &other)
			return *this;

		RemoveReference();
		mResource = other.mResource;
		mInterface = other.mInterface;
		AddReference();

		return *this;
	}

	~ResourceReference()
	{
		RemoveReference();
	}

	void Reset()
	{
		RemoveReference();
		mInterface = nullptr;
	}

	bool isReady() const { return mResource && mResource->getResourceStatus() == ResourceStatus::Ready; }
	static_assert(_MSC_VER == 1700, "Change to explicit operator bool()");
	operator bool() const { return mResource != nullptr; }
	typename T::Interface<T> *operator->() { return &mInterface; }
	const typename T::Interface<T> *operator->() const { return &mInterface; }

	template <typename O>
	friend class ResourceReference;

private:
	std::shared_ptr<T> mResource;
	typename T::Interface<T> mInterface;

	inline void AddReference()
	{
		if(mResource)
			mResource->AddReference();
	}

	inline void RemoveReference()
	{
		if(mResource)
			mResource->RemoveReference();
		mResource = nullptr;
	}
};

#endif // __ResourceReference_h__