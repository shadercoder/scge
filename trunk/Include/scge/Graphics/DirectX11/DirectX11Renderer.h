#ifndef __DirectX11Renderer_h__
#define __DirectX11Renderer_h__

#include "scge\Graphics\Renderer.h"
#include "scge\ResourceManager\ResourceFactory.h"

#include "scge\Graphics\DirectX11\DirectX11ComPtr.h"

#include <unordered_map>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#if defined(SCGE_DEBUG)
#pragma comment(lib, "d3dx11d.lib")
#else
#pragma comment(lib, "d3dx11.lib")
#endif

class DirectX11Renderer2D;
class DirectX11Renderer3D;
class DirectX11Renderer : public Renderer
{
public:
	DirectX11Renderer(Console &console, ResourceManager &resourceManager, FileSystem &fileSystem);
	virtual ~DirectX11Renderer();

	virtual bool Initialise(WindowHandle windowHandle, bool windowed, unsigned int width, unsigned int height);
	virtual void Release();

	virtual bool Render();

	virtual bool onResize(bool windowed, int width, int height);

	virtual Renderer2D &getRenderer2D() const;
	virtual Renderer3D &getRenderer3D() const;

private:
	std::unique_ptr<DirectX11Renderer2D> mRenderer2D;
	std::unique_ptr<DirectX11Renderer3D> mRenderer3D;

	bool mWindowed;
	unsigned int mWidth;
	unsigned int mHeight;

	ConsoleVariable<unsigned int, true> mMultiSampleLevel;
	ConsoleVariable<unsigned int> mRefreshRate;
	ConsoleVariable<bool> mVSync;

	ComPtr<ID3D11Device> mDevice;
	ComPtr<ID3D11DeviceContext> mDeviceContext;
	ComPtr<IDXGISwapChain> mSwapChain;

	ResourceFactory mTextureFactory;
	ResourceFactory mMaterialFactory;
	ResourceFactory mModelFactory;
	ResourceFactory mFontFactory;

	ResourceFactory mVertexShaderFactory;
	ResourceFactory mPixelShaderFactory;
	ResourceFactory mComputeShaderFactory;

	bool onInitialiseSwapChain();
	void onReleaseSwapChain();

	void CreateShaderDefines();
	std::unordered_map<std::string, std::string> mShaderDefineStrings;
	std::unique_ptr<D3D10_SHADER_MACRO[]> mShaderDefines;
};

#endif // __DirectX11Renderer_h__
