#ifndef DIRECTX11COMPTR_H_
#define DIRECTX11COMPTR_H_

#include "scge\Assert.h"

template <class T>
class ComPtr
{
public:
	ComPtr()
		: mPointer(nullptr)
	{}

	ComPtr(T* pointer)
		: mPointer(pointer)
	{
		if(mPointer) mPointer->AddRef();
	}

	~ComPtr()
	{
		if(mPointer) mPointer->Release();
	}

	ComPtr& operator=(T* pointer)
	{
		if(mPointer) mPointer->Release();
		mPointer = pointer;
		if(mPointer) mPointer->AddRef();

		return *this;
	}

	ComPtr& operator=(ComPtr<T> pointer)
	{
		if(mPointer) mPointer->Release();
		mPointer = pointer.mPointer;
		if(mPointer) mPointer->AddRef();

		return *this;
	}

	T* operator->()
	{
		SCGE_ASSERT(mPointer != nullptr);
		return mPointer;
	}

	const T* operator->() const
	{
		SCGE_ASSERT(mPointer != nullptr);
		return mPointer;
	}

	operator T*() const
	{
		return mPointer;
	}

	operator T*const *() const
	{
		return &mPointer;
	}

	T& operator*()
	{
		SCGE_ASSERT(mPointer != nullptr);
		return *mPointer;
	}

	const T& operator*() const
	{
		SCGE_ASSERT(mPointer != nullptr);
		return *mPointer;
	}

	T** operator&()
	{
		SCGE_ASSERT_MESSAGE(mPointer == nullptr, "Returning a pointer to the pointer is unsafe if it is set as it may be changed without being released");
		return &mPointer;
	}

	T*const * operator&() const
	{
		return &mPointer;
	}

	operator bool() const
	{
		return mPointer != nullptr;
	}

	void Release()
	{
		if(mPointer)
		{
			mPointer->Release();
			mPointer = nullptr;
		}
	}

	void Attach(T* pointer)
	{
		if(mPointer) mPointer->Release();
		mPointer = pointer;
	}

	T* Detach()
	{
		T* pointer = mPointer;
		mPointer = nullptr;
		return pointer;
	}

private:
	T* mPointer;
};

#endif /* DIRECTX11COMPTR_H_ */
