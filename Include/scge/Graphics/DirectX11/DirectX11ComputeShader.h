#ifndef DirectX11ComputeShader_h__
#define DirectX11ComputeShader_h__

#include "scge\FileSystem\FileResource.h"
#include "scge\Console.h"
#include "scge\Graphics\DirectX11\DirectX11ComPtr.h"

#include <d3d11.h>
#include <d3dx11.h>

class DirectX11ComputeShaderData : public FileResourceData
{
public:
	DirectX11ComputeShaderData(ID3D11Device *pd3dDevice, const D3D10_SHADER_MACRO *shaderDefines, Console &console, FileSystem &fileSystem, bool multiThreadLoad, const std::string &arguments);

	virtual std::string getIdentifier() const final;
	virtual std::string getFactory() const final { return "ComputeShader"; }

	virtual std::shared_ptr<Resource> createResource() const final;

	ID3D11Device *mD3D11Device;
	const D3D10_SHADER_MACRO *mShaderDefines;

	Console &mConsole;

	std::string mFunctionName;

	bool mMultiThreadLoad;
};

class DirectX11ComputeShader : public ResourceBase<DirectX11ComputeShaderData, FileResource>
{
public:
	DirectX11ComputeShader(const DirectX11ComputeShaderData *data) : ResourceBase<DirectX11ComputeShaderData, FileResource>(data) {}

	virtual bool Load() final;
	virtual bool Finalise() final;
	virtual void Release() final;

	ID3D11ComputeShader *getShader() const { return mComputeShader; }

	template <typename I>
	class Interface : public FileResource::Interface<I>
	{
	public:
		Interface(I *computeShaderResource) : FileResource::Interface<I>(computeShaderResource) {}

		ID3D11ComputeShader *getShader() const { return getResource().getShader(); }
	};

private:
	ComPtr<ID3D11ComputeShader> mComputeShader;

	ComPtr<ID3D10Blob> mShaderBuffer;

	bool finaliseLoad();
};

#endif // DirectX11ComputeShader_h__