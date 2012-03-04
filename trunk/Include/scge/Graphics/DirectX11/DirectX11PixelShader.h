#ifndef DirectX11PixelShader_h__
#define DirectX11PixelShader_h__

#include "scge\FileSystem\FileResource.h"
#include "scge\Console.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <atlbase.h>

class DirectX11PixelShaderData : public FileResourceData
{
public:
	DirectX11PixelShaderData(ID3D11Device *pd3dDevice, const D3D10_SHADER_MACRO *shaderDefines, Console &console, FileSystem &fileSystem, bool multiThreadLoad, const std::string &arguments);

	virtual std::string getIdentifier() const;
	virtual std::string getFactory() const { return "PixelShader"; }

	virtual std::shared_ptr<Resource> createResource() const;

	ID3D11Device *mD3D11Device;
	const D3D10_SHADER_MACRO *mShaderDefines;
	Console &mConsole;

	std::string mFunctionName;

	bool mMultiThreadLoad;
};

class DirectX11PixelShader : public ResourceBase<DirectX11PixelShaderData, FileResource>
{
public:
	DirectX11PixelShader(const DirectX11PixelShaderData *data) : ResourceBase<DirectX11PixelShaderData, FileResource>(data) {}

	virtual bool Load();
	virtual bool Finalise();
	virtual void Release();

	ID3D11PixelShader *getShader() const { return mPixelShader; }

	template <typename I>
	class Interface : public FileResource::Interface<I>
	{
	public:
		Interface(I *pixelShaderResource) : FileResource::Interface<I>(pixelShaderResource) {}

		ID3D11PixelShader *getShader() const { return getResource().getShader(); }
	};

private:
	CComPtr<ID3D11PixelShader> mPixelShader;

	CComPtr<ID3D10Blob> mShaderBuffer;

	bool finaliseLoad();
};

#endif // DirectX11PixelShader_h__