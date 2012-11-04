#ifndef __DirectX11Texture_h__
#define __DirectX11Texture_h__

#include "scge\Graphics\Base\TextureResource.h"
#include "scge\Console.h"

#include "scge\Graphics\DirectX11\DirectX11ComPtr.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <vector>

class DirectX11TextureData : public TextureResourceData
{
public:
	DirectX11TextureData(ID3D11Device *d3d11Device, Console &console, FileSystem &fileSystem, bool multiThreadLoad, const std::string &arguments);

	virtual std::unique_ptr<Resource> createResource() const final;

	ID3D11Device *mD3D11Device;
	Console &mConsole;

	bool mMultiThreadLoad;
};

class DirectX11Texture : public ResourceBase<DirectX11TextureData, TextureResource>
{
public:
	DirectX11Texture(const DirectX11TextureData *data) : ResourceBase(data) {}

	virtual bool Load() final;
	virtual bool Finalise() final;
	virtual void Release() final;

	ID3D11ShaderResourceView* GetShaderResourceView() const { return mTextureSRV; }
	ID3D11ShaderResourceView *const *GetShaderResourceViewPointer() const { return &mTextureSRV; }

	template <typename I>
	class Interface : public TextureResource::Interface<I>
	{
	public:
		Interface(I *fileResource) : TextureResource::Interface<I>(fileResource) {}

		ID3D11ShaderResourceView* GetShaderResourceView() const { return getResource().GetShaderResourceView(); }
		ID3D11ShaderResourceView *const *GetShaderResourceViewPointer() const { return getResource().GetShaderResourceViewPointer(); }
	};

private:
	ComPtr<ID3D11ShaderResourceView> mTextureSRV;

	std::vector<char> mLoadingData;

	bool finaliseLoad();
};

#endif // __DirectX11Texture_h__