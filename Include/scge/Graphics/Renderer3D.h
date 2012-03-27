#ifndef __Renderer3D_h__
#define __Renderer3D_h__

#include "scge\Math\Camera.h"
#include "scge\Graphics\Base\TextureResource.h"
#include "scge\ResourceManager\ResourceReference.h"

class Console;
class ResourceManager;
class PointLight;
class Renderer3D
{
public:
	Renderer3D(Console &console, ResourceManager &resourceManager)
		: mConsole(console)
		, mResourceManager(resourceManager)
	{}

	virtual ~Renderer3D(){}

	Camera &getCamera() { return mCamera; }
	const Camera &getCamera() const { return mCamera; }
	void setCamera(const Camera &camera) { mCamera = camera; }

	virtual ResourceReference<TextureResource> getSkybox() const = 0;
	virtual void setSkybox(const ResourceReference<TextureResource> &skyboxTexture) = 0;

	virtual void registerPointLight(PointLight &pointLight) = 0;
	virtual void unRegisterPointLight(PointLight &pointLight) = 0;

protected:
	Console &mConsole;
	ResourceManager &mResourceManager;

	Camera mCamera;

private:
	Renderer3D(const Renderer3D &);
	Renderer3D &operator=(const Renderer3D &);
};

#endif // __Renderer3D_h__