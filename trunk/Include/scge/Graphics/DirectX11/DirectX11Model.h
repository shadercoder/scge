#ifndef DirectX11Model_h__
#define DirectX11Model_h__

#include "scge\Config.h"
#include "scge\Graphics\Base\ModelResource.h"
#include "scge\ResourceManager.h"
#include "scge\Graphics\DirectX11\DirectX11Mesh.h"
#include "scge\Console.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <atlbase.h>
#include <vector>

class DirectX11ModelData : public ModelResourceData
{
public:
	DirectX11ModelData(ID3D11Device *d3d11Device, Console &console, ResourceManager &resourceManager, FileSystem &fileSystem, bool multiThreadLoad, const std::string &arguments);

	virtual std::shared_ptr<Resource> createResource() const;

	ID3D11Device *mD3D11Device;
	ResourceManager &mResourceManager;
	Console &mConsole;

	bool mMultiThreadLoad;
};

class DirectX11Model : public ResourceBase<DirectX11ModelData, ModelResource>
{
public:
	DirectX11Model(const DirectX11ModelData *data) : ResourceBase(data) {}

	virtual bool Load();
	virtual bool Finalise();
	virtual void Release();

	virtual ResourceStatus getResourceStatus() const;

	virtual unsigned int numMeshes() const { return mMeshs.size(); }

	virtual const MeshResource &getMesh(unsigned int meshNumber) const { return *mMeshs[meshNumber].get(); }

	void Render(ID3D11DeviceContext* deviceContext) const;

	template <typename I>
	class Interface : public ModelResource::Interface<I>
	{
	public:
		Interface(I *fileResource) : ModelResource::Interface<I>(fileResource) {}

		void Render(ID3D11DeviceContext* deviceContext) const { getResource().Render(deviceContext); }
	};

private:
	std::vector<std::unique_ptr<DirectX11Mesh>> mMeshs;
};

#endif // DirectX11Model_h__