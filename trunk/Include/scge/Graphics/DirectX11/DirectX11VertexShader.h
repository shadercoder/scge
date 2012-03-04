#ifndef DirectX11VertexShader_h__
#define DirectX11VertexShader_h__

#include "scge\FileSystem\FileResource.h"
#include "scge\Console.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <atlbase.h>

class DirectX11VertexShaderData : public FileResourceData
{
public:
	DirectX11VertexShaderData(ID3D11Device *pd3dDevice, const D3D10_SHADER_MACRO *shaderDefines, Console &console, FileSystem &fileSystem, bool multiThreadLoad, const std::string &arguments);

	virtual std::string getIdentifier() const;
	virtual std::string getFactory() const { return "VertexShader"; }

	virtual std::shared_ptr<Resource> createResource() const;

	ID3D11Device *mD3D11Device;
	const D3D10_SHADER_MACRO *mShaderDefines;
	Console &mConsole;

	std::string mLayoutName;
	std::string mFunctionName;

	bool mMultiThreadLoad;
};

class DirectX11VertexShader : public ResourceBase<DirectX11VertexShaderData, FileResource>
{
public:
	DirectX11VertexShader(const DirectX11VertexShaderData *data) : ResourceBase<DirectX11VertexShaderData, FileResource>(data) {}

	virtual bool Load();
	virtual bool Finalise();
	virtual void Release();

	ID3D11VertexShader *getShader() const { return mVertexShader; }
	ID3D11InputLayout *getInputLayout() const { return mVertexLayout; }

	template <typename I>
	class Interface : public FileResource::Interface<I>
	{
	public:
		Interface(I *vertexShaderResource) : FileResource::Interface<I>(vertexShaderResource) {}
		
		ID3D11VertexShader *getShader() const { return getResource().getShader(); }
		ID3D11InputLayout *getInputLayout() const { return getResource().getInputLayout(); }
	};

private:
	CComPtr<ID3D11VertexShader> mVertexShader;
	CComPtr<ID3D11InputLayout> mVertexLayout;

	CComPtr<ID3D10Blob> mShaderBuffer;

	bool finaliseLoad();
};

#endif // DirectX11VertexShader_h__