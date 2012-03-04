#ifndef __Renderer_h__
#define __Renderer_h__

#include "scge\Console.h"
#include "scge\ResourceManager.h"
#include "scge\FileSystem.h"
#include "scge\Graphics\Renderer2D.h"
#include "scge\Graphics\Renderer3D.h"

#include "scge\Window\WindowHandle.h"

class Renderer
{
public:
	Renderer(Console &console, ResourceManager &resourceManager, FileSystem &fileSystem)
		: mConsole(console)
		, mResourceManager(resourceManager)
		, mFileSystem(fileSystem)
	{}

	virtual ~Renderer(){}

	virtual bool Initialise(WindowHandle windowHandle, bool windowed, unsigned int width, unsigned int height) = 0;
	virtual void Release() = 0;

	virtual bool Render() = 0;

	virtual bool onResize(bool windowed, int width, int height) = 0;

	virtual Renderer2D &getRenderer2D() const = 0;
	virtual Renderer3D &getRenderer3D() const = 0;

protected:
	Console &mConsole;
	ResourceManager &mResourceManager;
	FileSystem &mFileSystem;

private:
	Renderer(const Renderer &);
	Renderer &operator=(const Renderer &);
};

#endif // __Renderer_h__