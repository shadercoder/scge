#ifndef __Renderer2D_h__
#define __Renderer2D_h__

#include "scge\Console.h"
#include "scge\ResourceManager.h"

class Renderer2D
{
public:
	Renderer2D(Console &console, ResourceManager &resourceManager)
		: mConsole(console)
		, mResourceManager(resourceManager)
	{}

	virtual ~Renderer2D(){}

protected:
	Console &mConsole;
	ResourceManager &mResourceManager;

private:
	Renderer2D(const Renderer2D &);
	Renderer2D &operator=(const Renderer2D &);
};

#endif // __Renderer2D_h__