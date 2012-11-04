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
	ResourceReference(const std::unique_ptr<O> &resource)
		: mResource(dynamic_cast<T*>(resource.get()))
		, mInterface(mResource)
	{
		static_assert(std::is_convertible<O, Resource>::value, "ResourceReference can only point to a Resource Type");
		AddReference();
	}

	template <typename O>
	ResourceReference(const ResourceReference<O> &other)
		: mResource(dynamic_cast<T*>(other.mResource))
		, mInterface(mResource)
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
		mResource = dynamic_cast<T*>(other.mResource);
		mInterface = typename T::template Interface<T>(mResource);
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
	explicit operator bool() const { return mResource != nullptr; }
	typename T::template Interface<T> *operator->() { return &mInterface; }
	const typename T::template Interface<T> *operator->() const { return &mInterface; }

	template <typename O>
	friend class ResourceReference;

private:
	T *mResource;
	typename T::template Interface<T> mInterface;

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
