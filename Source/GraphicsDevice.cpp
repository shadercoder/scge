#include "scge\GraphicsDevice.h"

#if defined(SCGE_SYSTEM_WINDOWS)
#include "scge\Graphics\DirectX11\DirectX11Renderer.h"
#endif

GraphicsDevice::GraphicsDevice(Console &console, ResourceManager &resourceManager, FileSystem &fileSystem)
	: mInitialised(false)
	, mConsole(console)
	, mResourceManager(resourceManager)
	, mFileSystem(fileSystem)
#if defined(SCGE_SYSTEM_WINDOWS)
	, mRendererAPI(console, "Graphics.RendererAPI", "", "DirectX11")
#else
	, mRendererAPI(console, "Graphics.RendererAPI", "", "OpenGL")
#endif
{
}

bool GraphicsDevice::Initialise(WindowHandle windowHandle, bool windowed, unsigned int width, unsigned int height)
{
	if(mInitialised)
		return true;

	mRendererAPI.updateValue();
#if defined(SCGE_SYSTEM_WINDOWS)
	if(mRendererAPI.getValue() == "DirectX11")
		mRenderer = std::unique_ptr<Renderer>(new DirectX11Renderer(mConsole, mResourceManager, mFileSystem));
	else
#endif
	{
		mConsole.printError("Do not know API " + mRendererAPI.getValue());
		return true;
	}

	if(!mRenderer || mRenderer->Initialise(windowHandle, windowed, width, height))
		return true;

	mInitialised = true;

	return false;
}

void GraphicsDevice::Release()
{
	if(!mInitialised)
		return;

	mRenderer->Release();
	mRenderer.reset();

	mInitialised = false;
}

bool GraphicsDevice::Render()
{
	if(!mInitialised)
		return true;

	return mRenderer->Render();
}

bool GraphicsDevice::onResize(bool windowed, int width, int height)
{
	if(mInitialised)
		return mRenderer->onResize(windowed, width, height);

	return false;
}