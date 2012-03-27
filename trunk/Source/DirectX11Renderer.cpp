#include "scge\Graphics\DirectX11\DirectX11Renderer.h"

#include "scge\Graphics\DirectX11\DirectX11Renderer2D.h"
#include "scge\Graphics\DirectX11\DirectX11Renderer3D.h"

#include "scge\Utility.h"

#include "scge\Graphics\DirectX11\DirectX11Texture.h"
#include "scge\Graphics\DirectX11\DirectX11Material.h"
#include "scge\Graphics\DirectX11\DirectX11Model.h"
#include "scge\Graphics\DirectX11\DirectX11Font.h"
#include "scge\Graphics\DirectX11\DirectX11VertexShader.h"
#include "scge\Graphics\DirectX11\DirectX11PixelShader.h"
#include "scge\Graphics\DirectX11\DirectX11ComputeShader.h"

bool QueryDisplayMode(bool windowed, unsigned int width, unsigned int height, int desiredRefreshRate, DXGI_MODE_DESC &chosenMode)
{
	ComPtr<IDXGIFactory1> factory;
	if(FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory)))
		return true;

	ComPtr<IDXGIAdapter> adapter;
	if(FAILED(factory->EnumAdapters(0, adapter.getModifieablePointer())))
		return true;

	ComPtr<IDXGIOutput> adapterOutput;
	if(FAILED(adapter->EnumOutputs(0, adapterOutput.getModifieablePointer())))
		return true;

	unsigned int numModes = 0;
	if(FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr)))
		return true;

	std::vector<DXGI_MODE_DESC> displayModeList(numModes);
	if(FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList.data())))
		return true;

	int bestScaling = -1;
	int bestRefreshRate = -1;
	for(unsigned int i = 0; i < numModes; ++i)
	{
		if(displayModeList[i].Width != width || displayModeList[i].Height != height)
			continue;

		const int refreshRate = displayModeList[i].RefreshRate.Numerator / displayModeList[i].RefreshRate.Denominator;
		if(desiredRefreshRate <= 0)
		{
			if(refreshRate < bestRefreshRate)
				continue;
		}
		else
		{
			if(refreshRate > desiredRefreshRate)
				continue;

			if(bestRefreshRate != -1 && refreshRate < bestRefreshRate)
				continue;
		}

		if(displayModeList[i].Scaling != DXGI_MODE_SCALING_CENTERED && bestScaling != -1)
		{
			if(bestScaling == DXGI_MODE_SCALING_STRETCHED && displayModeList[i].Scaling != DXGI_MODE_SCALING_UNSPECIFIED)
				continue;
		}

		bestScaling = displayModeList[i].Scaling;
		bestRefreshRate = refreshRate;
		chosenMode = displayModeList[i];
	}

	if(bestScaling == -1)
		return true;

	return false;
}

DirectX11Renderer::DirectX11Renderer(Console &console, ResourceManager &resourceManager, FileSystem &fileSystem)
	: Renderer(console, resourceManager, fileSystem)
	, mMultiSampleLevel(console, "Graphics.MultiSampleLevel", "", 1, 8, 2)
	, mRefreshRate("Graphics.RefreshRate", "")
	, mVSync(console, "Graphics.VSync", "", false)
	, mRenderer2D(new DirectX11Renderer2D(console, resourceManager))
	, mRenderer3D(new DirectX11Renderer3D(console, resourceManager))
	, mTextureFactory(resourceManager, "Texture")
	, mMaterialFactory(resourceManager, "Material")
	, mModelFactory(resourceManager, "Model")
	, mFontFactory(resourceManager, "Font")
	, mVertexShaderFactory(resourceManager, "VertexShader")
	, mPixelShaderFactory(resourceManager, "PixelShader")
	, mComputeShaderFactory(resourceManager, "ComputeShader")
{
	DXGI_MODE_DESC mode;
	if(!QueryDisplayMode(false, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), 0, mode))
		mRefreshRate.initialise(console, mode.RefreshRate.Numerator / mode.RefreshRate.Denominator);
	else
		mRefreshRate.initialise(console, 60);
}

DirectX11Renderer::~DirectX11Renderer()
{
}

bool DirectX11Renderer::Initialise(WindowHandle windowHandle, bool windowed, unsigned int width, unsigned int height)
{
	mWindowed = windowed;
	mWidth = width;
	mHeight = height;

	mMultiSampleLevel.updateValue();
	mRefreshRate.updateValue();
	mVSync.updateValue();

	ComPtr<IDXGIFactory1> factory;
	if(FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory)))
		return true;

	if(FAILED(factory->MakeWindowAssociation(static_cast<HWND>(windowHandle), DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_PRINT_SCREEN)))
		return true;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {0};
	swapChainDesc.BufferCount = 1;

	if(QueryDisplayMode(windowed, width, height, mRefreshRate, swapChainDesc.BufferDesc))
		return true;

	if(windowed || !mVSync)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	swapChainDesc.OutputWindow = static_cast<HWND>(windowHandle);
	swapChainDesc.SampleDesc.Count = mMultiSampleLevel;
	swapChainDesc.Windowed = windowed;
	swapChainDesc.Flags = windowed ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT flags = 0;
#if defined(SCGE_DEBUG)
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	if(FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, &featureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc, mSwapChain.getModifieablePointer(), mDevice.getModifieablePointer(), nullptr, mDeviceContext.getModifieablePointer())))
	{
		mConsole.printError("D3D11CreateDeviceAndSwapChain() failed");
		return true;
	}

	CreateShaderDefines();

	D3D11_FEATURE_DATA_THREADING threadingSupport;
	if(FAILED(mDevice->CheckFeatureSupport(D3D11_FEATURE_THREADING, &threadingSupport, sizeof(D3D11_FEATURE_DATA_THREADING))))
		return true;

	const bool multiThreadLoad = threadingSupport.DriverConcurrentCreates != 0;

	if(mTextureFactory.Initialise([this, multiThreadLoad](const std::string &arguments){
		return std::unique_ptr<ResourceData>(new DirectX11TextureData(mDevice, mConsole, mFileSystem, multiThreadLoad, arguments));
	}))
		return true;

	if(mMaterialFactory.Initialise([this](const std::string &arguments){
		return std::unique_ptr<ResourceData>(new DirectX11MaterialData(mConsole, mResourceManager, mFileSystem, arguments));
	}))
		return true;

	if(mModelFactory.Initialise([this, multiThreadLoad](const std::string &arguments){
		return std::unique_ptr<ResourceData>(new DirectX11ModelData(mDevice, *mRenderer3D, mConsole, mResourceManager, mFileSystem, multiThreadLoad, arguments));
	}))
		return true;

	if(mFontFactory.Initialise([this](const std::string &arguments){
		return std::unique_ptr<ResourceData>(new DirectX11FontData(mDevice, mFileSystem, arguments));
	}))
		return true;

	if(mVertexShaderFactory.Initialise([this, multiThreadLoad](const std::string &arguments){
		return std::unique_ptr<ResourceData>(new DirectX11VertexShaderData(mDevice, mShaderDefines.data(), mConsole, mFileSystem, multiThreadLoad, arguments));
	}))
		return true;

	if(mPixelShaderFactory.Initialise([this, multiThreadLoad](const std::string &arguments){
		return std::unique_ptr<ResourceData>(new DirectX11PixelShaderData(mDevice, mShaderDefines.data(), mConsole, mFileSystem, multiThreadLoad, arguments));
	}))
		return true;

	if(mComputeShaderFactory.Initialise([this, multiThreadLoad](const std::string &arguments){
		return std::unique_ptr<ResourceData>(new DirectX11ComputeShaderData(mDevice, mShaderDefines.data(), mConsole, mFileSystem, multiThreadLoad, arguments));
	}))
		return true;

	if(mRenderer2D->onInitialiseDevice(mDevice, mDeviceContext))
	{
		mConsole.printError("mRenderer2D->Initialise() failed");
		return true;
	}

	if(mRenderer3D->onInitialiseDevice(mDevice, mDeviceContext))
	{
		mConsole.printError("mRenderer3D->Initialise() failed");
		return true;
	}

	if(onInitialiseSwapChain())
		return true;

	return false;
}

void DirectX11Renderer::Release()
{
	onReleaseSwapChain();

	mRenderer3D->onReleaseDevice();
	mRenderer2D->onReleaseDevice();

	mComputeShaderFactory.Release();
	mPixelShaderFactory.Release();
	mVertexShaderFactory.Release();
	mFontFactory.Release();
	mModelFactory.Release();
	mMaterialFactory.Release();
	mTextureFactory.Release();

	mSwapChain.Release();
	mDeviceContext.Release();
	mDevice.Release();
}

bool DirectX11Renderer::onInitialiseSwapChain()
{
	if(mRenderer2D->onInitialiseSwapChain(mSwapChain, mWindowed, mWidth, mHeight))
		return true;

	if(mRenderer3D->onInitialiseSwapChain(mSwapChain, mWindowed, mWidth, mHeight))
		return true;

	CD3D11_VIEWPORT viewport(0.0f, 0.0f, static_cast<float>(mWidth), static_cast<float>(mHeight));
	mDeviceContext->RSSetViewports(1, &viewport);

	return false;
}

void DirectX11Renderer::onReleaseSwapChain()
{
	mRenderer3D->onReleaseSwapChain();
	mRenderer2D->onReleaseSwapChain();
}

bool DirectX11Renderer::Render()
{
	if(mRenderer3D->Render())
	{
		mConsole.printError("mRenderer3D->Render() failed");
		return true;
	}

	if(mRenderer2D->Render())
	{
		mConsole.printError("mRenderer3D->Render() failed");
		return true;
	}

	HRESULT hr = mSwapChain->Present(mVSync ? 1 : 0, 0);
	if(FAILED(hr))
	{
		mConsole.printError("Present() failed");
		return true;
	}

	return false;
}

bool DirectX11Renderer::onResize(bool windowed, int width, int height)
{
	onReleaseSwapChain();

	bool changedWindowedMode = mWindowed != windowed;
	bool changedResolution = mWidth != width || mHeight != height || mRefreshRate.hasValueChanged() || mVSync.hasValueChanged();

	mRefreshRate.updateValue();
	mVSync.updateValue();

	if(changedWindowedMode)
	{
		DXGI_MODE_DESC newMode;
		if(QueryDisplayMode(windowed, width, height, mRefreshRate, newMode))
			return true;

		if(windowed || !mVSync)
		{
			newMode.RefreshRate.Numerator = 0;
			newMode.RefreshRate.Denominator = 1;
		}

		if(FAILED(mSwapChain->ResizeTarget(&newMode)))
			return true;

		if(FAILED(mSwapChain->SetFullscreenState(!windowed, nullptr)))
		{
			mConsole.printError("SetFullscreenState() failed");
			return true;
		}

		mWindowed = windowed;
	}

	if(FAILED(mSwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, mWindowed ? 0 : DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH)))
	{
		mConsole.printError("ResizeBuffers() failed");
		return true;
	}

	mWidth = width;
	mHeight = height;

	if(onInitialiseSwapChain())
		return true;

	return false;
}

Renderer2D &DirectX11Renderer::getRenderer2D() const
{
	SCGE_ASSERT(mRenderer2D);
	return *(mRenderer2D.get());
}

Renderer3D &DirectX11Renderer::getRenderer3D() const
{
	SCGE_ASSERT(mRenderer3D);
	return *(mRenderer3D.get());
}

void DirectX11Renderer::CreateShaderDefines()
{
	mShaderDefineStrings.clear();
	mShaderDefineStrings["MSAA_SAMPLES"] = StringUtility::toString(mMultiSampleLevel.getValue());

	mShaderDefines.clear();
	mShaderDefines.reserve(mShaderDefineStrings.size() + 1);
	for(auto & shaderDefineString : mShaderDefineStrings)
	{
		D3D10_SHADER_MACRO define = { shaderDefineString.first.c_str(), shaderDefineString.second.c_str() };
		mShaderDefines.push_back(define);
	}

	D3D10_SHADER_MACRO define = { nullptr, nullptr };
	mShaderDefines.push_back(define);
}