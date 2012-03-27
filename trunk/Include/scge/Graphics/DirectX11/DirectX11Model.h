#ifndef DirectX11Model_h__
#define DirectX11Model_h__

#include "scge\Config.h"
#include "scge\Graphics\Base\ModelResource.h"
#include "scge\ResourceManager.h"
#include "scge\Graphics\DirectX11\DirectX11Mesh.h"
#include "scge\Console.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <vector>

class DirectX11Renderer3D;
class DirectX11ModelData : public ModelResourceData
{
public:
	DirectX11ModelData(ID3D11Device *d3d11Device, DirectX11Renderer3D &directX11Renderer3D, Console &console, ResourceManager &resourceManager, FileSystem &fileSystem, bool multiThreadLoad, const std::string &arguments);

	virtual std::shared_ptr<Resource> createResource() const final;

	ID3D11Device *mD3D11Device;
	DirectX11Renderer3D &mDirectX11Renderer3D;
	ResourceManager &mResourceManager;
	Console &mConsole;

	bool mMultiThreadLoad;
};

class DirectX11Model : public ResourceBase<DirectX11ModelData, ModelResource>
{
public:
	DirectX11Model(const DirectX11ModelData *data) : ResourceBase(data) {}

	virtual bool Load() final;
	virtual bool Finalise() final;
	virtual void Release() final;

	virtual ResourceStatus getResourceStatus() const final;

	virtual unsigned int numMeshes() const final { return mMeshs.size(); }

	virtual const MeshResource &getMesh(unsigned int meshNumber) const final { return *mMeshs[meshNumber].get(); }

	virtual void Render() const final;
	void Render(ID3D11DeviceContext* deviceContext, bool alphaTestPass) const;

	template <typename I>
	class Interface : public ModelResource::Interface<I>
	{
	public:
		Interface(I *fileResource) : ModelResource::Interface<I>(fileResource) {}
	};

private:
	std::vector<std::unique_ptr<DirectX11Mesh>> mMeshs;
};

#endif // DirectX11Model_h__