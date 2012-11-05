#ifndef DirectX11StructuredBufferHelper_h__
#define DirectX11StructuredBufferHelper_h__

#include "scge\Graphics\DirectX11\DirectX11ComPtr.h"

#include <d3d11.h>
#include <vector>

template <typename T>
class DirectX11BufferHelper
{
public:
	DirectX11BufferHelper() : mElements(0) {}
	~DirectX11BufferHelper() { Release(); }

	DirectX11BufferHelper(DirectX11BufferHelper &&other)
	{
		operator=(std::move(other));
	}

	DirectX11BufferHelper& operator=(DirectX11BufferHelper &&other)
	{
		if(this != &other)
		{
			std::swap(mElements, other.mElements);
			std::swap(mBuffer, other.mBuffer);
			std::swap(mShaderResource, other.mShaderResource);
			std::swap(mUnorderedAccess, other.mUnorderedAccess);
		}

		return *this;
	}

	bool Initialise(ID3D11Device* d3dDevice);
	bool Initialise(ID3D11Device* d3dDevice, int elements, UINT bindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, bool structured = false, T *initialValues = nullptr, bool dynamic = false);
	void Release();

	ID3D11Buffer *GetBuffer() const { return mBuffer; }
	ID3D11Buffer * const *GetBufferPointer() const { return &mBuffer; }

	ID3D11UnorderedAccessView *GetUnorderedAccess() const { return mUnorderedAccess; }
	ID3D11UnorderedAccessView * const *GetUnorderedAccessPointer() const { return &mUnorderedAccess; }

	ID3D11ShaderResourceView *GetShaderResource() const { return mShaderResource; }
	ID3D11ShaderResourceView * const *GetShaderResourcePointer() const { return &mShaderResource; }

	T *MapDiscard(ID3D11DeviceContext* d3dDeviceContext) const;
	void Unmap(ID3D11DeviceContext* d3dDeviceContext) const;

private:
	int mElements;
	ComPtr<ID3D11Buffer> mBuffer;
	ComPtr<ID3D11ShaderResourceView> mShaderResource;
	ComPtr<ID3D11UnorderedAccessView> mUnorderedAccess;

	DirectX11BufferHelper(const DirectX11BufferHelper&);
	DirectX11BufferHelper& operator=(const DirectX11BufferHelper&);
};

inline unsigned int CBSize(unsigned int size)
{
	unsigned int cbsize = size + (16 - (size % 16));
	return cbsize;
}

template <typename T>
bool DirectX11BufferHelper<T>::Initialise(ID3D11Device* d3dDevice)
{
	mElements = 0;

	CD3D11_BUFFER_DESC desc(CBSize(sizeof(T)), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
	if(FAILED(d3dDevice->CreateBuffer(&desc, 0, mBuffer.getModifiablePointer())))
		return true;

	return false;
}

template <typename T>
bool DirectX11BufferHelper<T>::Initialise(ID3D11Device* d3dDevice, int elements, UINT bindFlags, bool structured, T *initialValues, bool dynamic)
{
	mElements = elements;

	CD3D11_BUFFER_DESC desc(sizeof(T) * elements, bindFlags, dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT, dynamic ? D3D11_CPU_ACCESS_WRITE : 0, structured ? D3D11_RESOURCE_MISC_BUFFER_STRUCTURED : 0, structured ? sizeof(T) : 0);
	
	if(initialValues)
	{
		D3D11_SUBRESOURCE_DATA data = {0};
		data.pSysMem = initialValues;
		if(FAILED(d3dDevice->CreateBuffer(&desc, &data, mBuffer.getModifiablePointer())))
			return true;
	}
	else
	{
		if(FAILED(d3dDevice->CreateBuffer(&desc, 0, mBuffer.getModifiablePointer())))
			return true;
	}

	if(bindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		if(FAILED(d3dDevice->CreateUnorderedAccessView(mBuffer, 0, mUnorderedAccess.getModifiablePointer())))
			return true;
	}

	if(bindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		if(FAILED(d3dDevice->CreateShaderResourceView(mBuffer, 0, mShaderResource.getModifiablePointer())))
			return true;
	}

	return false;
}

template <typename T>
void DirectX11BufferHelper<T>::Release()
{
	mUnorderedAccess.Release();
	mShaderResource.Release();
	mBuffer.Release();
	mElements = 0;
}

template <typename T>
T *DirectX11BufferHelper<T>::MapDiscard(ID3D11DeviceContext* d3dDeviceContext) const
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	d3dDeviceContext->Map(mBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	return static_cast<T*>(mappedResource.pData);
}

template <typename T>
void DirectX11BufferHelper<T>::Unmap(ID3D11DeviceContext* d3dDeviceContext) const
{
	d3dDeviceContext->Unmap(mBuffer, 0);
}

#endif // DirectX11StructuredBufferHelper_h__