#ifndef DirectX11Mesh_h__
#define DirectX11Mesh_h__

#include "scge\ResourceManager.h"
#include "scge\Graphics\DirectX11\DirectX11Material.h"
#include "scge\Graphics\Base\MeshResource.h"
#include "scge\Graphics\DirectX11\DirectX11BufferHelper.h"
#include "scge\Console.h"
#include "scge\Math\Vector2.h"
#include "scge\Math\Vector3.h"
#include "scge\Math\Matrix3.h"
#include "scge\Math\Matrix4.h"
#include "scge\Config.h"

#include <d3d11.h>
#include <d3dx11.h>
#include <vector>
#include <istream>

class DirectX11Mesh : public MeshResource
{
public:
	DirectX11Mesh(ID3D11Device *d3d11Device, Console &console, ResourceManager &resourceManager, FileSystem &fileSystem);

	bool Load(std::ifstream &file);
	bool Finalise();
	bool finaliseLoad();
	void Release();

	ResourceStatus getResourceStatus() const;

	void Render(ID3D11DeviceContext* deviceContext, bool alphaTestPass) const;

	virtual ResourceReference<MaterialResource> getMaterial() const { return mMaterial; }

	struct VertexType
	{
		Vector3 position;
		Vector2 texture;
		Vector3 normal;
		Vector3 binormal;
		Vector3 tangent;
	};

private:
	ID3D11Device *mD3D11Device;
	ResourceManager &mResourceManager;
	FileSystem &mFileSystem;
	Console &mConsole;

	std::string mMaterialName;
	ResourceReference<DirectX11MaterialResource> mMaterial;
	
	DirectX11BufferHelper<VertexType> mVertexBuffer;

	uint32 mIndexCount;
	DirectX11BufferHelper<unsigned long> mIndexBuffer;

	std::vector<VertexType> mVertices;
	std::vector<unsigned long> mIndices;
};

#endif // DirectX11Mesh_h__