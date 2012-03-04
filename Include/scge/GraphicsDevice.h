#ifndef __GraphicsDevice_h__
#define __GraphicsDevice_h__

#include "scge\Console.h"
#include "scge\ResourceManager.h"
#include "scge\Graphics\Renderer.h"

#include "scge\Window\WindowHandle.h"

class GraphicsDevice
{
public:
	GraphicsDevice(Console &console, ResourceManager &resourceManager, FileSystem &fileSystem);

	bool Initialise(WindowHandle windowHandle, bool windowed, unsigned int width, unsigned int height);
	void Release();

	bool Render();

	bool onResize(bool windowed, int width, int height);

	Renderer &getRenderer() const { return *mRenderer; }
	Renderer2D &getRenderer2D() const { return mRenderer->getRenderer2D(); }
	Renderer3D &getRenderer3D() const { return mRenderer->getRenderer3D(); }

private:
	Console &mConsole;
	ResourceManager &mResourceManager;
	FileSystem &mFileSystem;

	bool mInitialised;

	ConsoleVariable<std::string> mRendererAPI;
	std::unique_ptr<Renderer> mRenderer;

	GraphicsDevice(const GraphicsDevice &);
	GraphicsDevice &operator=(const GraphicsDevice &);
};

#endif // __GraphicsDevice_h__