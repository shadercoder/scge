#include "scge\Graphics\DirectX11\DirectX11Mesh.h"

#include "scge\Graphics\DirectX11\DirectX11Texture.h"
#include "scge\Config.h"

DirectX11Mesh::DirectX11Mesh(ID3D11Device *d3d11Device, Console &console, ResourceManager &resourceManager, FileSystem &fileSystem)
	: mD3D11Device(d3d11Device)
	, mConsole(console)
	, mResourceManager(resourceManager)
	, mFileSystem(fileSystem)
{
}

template <typename T>
inline bool read(std::ifstream &file, T &result)
{
	file.read((char *)&result, sizeof(T));
	return file.fail();
}

inline bool read(std::ifstream &file, std::string &result)
{
	uint32 materialNameLength = 0;
	if(read(file, materialNameLength)) return true;

	result.resize(materialNameLength);

	file.read(&result[0], sizeof(std::string::value_type)*materialNameLength);
	return file.fail();
}

bool DirectX11Mesh::Load(std::ifstream &file)
{
	if(read(file, mMaterialName)) return true;

	uint32 vertexCount = 0;
	if(read(file, vertexCount)) return true;
	mVertices.resize(vertexCount);

	for(size_t i = 0; i < vertexCount; ++i)
	{
		if(read(file, mVertices[i]))
			return true;
	}

	mIndexCount = 0;
	if(read(file, mIndexCount)) return true;
	mIndices.resize(mIndexCount);

	for(size_t i = 0; i < mIndexCount; ++i)
	{
		if(read(file, mIndices[i]))
			return true;
	}

	return false;
}

bool DirectX11Mesh::Finalise()
{
	mMaterial = mResourceManager.getResourceReference<MaterialResource>(mMaterialName);

	if(!mMaterial) return true;
	mMaterialName.clear();

	return false;
}

bool DirectX11Mesh::finaliseLoad()
{
	if(mVertexBuffer.Initialise(mD3D11Device, mVertices.size(), D3D11_BIND_VERTEX_BUFFER, false, mVertices.data())) return true;
	mVertices.clear();

	if(mIndexBuffer.Initialise(mD3D11Device, mIndices.size(), D3D11_BIND_INDEX_BUFFER, false, mIndices.data())) return true;
	mIndices.clear();

	return false;
}

void DirectX11Mesh::Release()
{
	mIndexBuffer.Release();
	mVertexBuffer.Release();

	mIndices.clear();
	mVertices.clear();

	mMaterialName.clear();
	mMaterial.Reset();
}

void DirectX11Mesh::Render(ID3D11DeviceContext* deviceContext) const
{
	const static unsigned int stride = sizeof(VertexType);
	const static unsigned int offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetBufferPointer(), &stride, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer.GetBuffer(), DXGI_FORMAT_R32_UINT, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ResourceReference<DirectX11Texture> diffuse = mMaterial->getDiffuseReference();
	ResourceReference<DirectX11Texture> normal = mMaterial->getNormalReference();
	ResourceReference<DirectX11Texture> specular = mMaterial->getSpecularReference();

	ID3D11ShaderResourceView* aRViews[] = { diffuse->GetShaderResourceView(), normal->GetShaderResourceView(), specular->GetShaderResourceView() };
	deviceContext->PSSetShaderResources(0, 3, aRViews);

	deviceContext->DrawIndexed(mIndexCount, 0, 0);
}

ResourceStatus DirectX11Mesh::getResourceStatus() const
{
	return mMaterial->getResourceStatus();
}