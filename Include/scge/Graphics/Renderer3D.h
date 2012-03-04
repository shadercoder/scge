#ifndef __Renderer3D_h__
#define __Renderer3D_h__

#include "scge\Console.h"
#include "scge\ResourceManager.h"
#include "scge\Math\Camera.h"
#include "scge\Graphics\Base\TextureResource.h"

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

	const ResourceReference<TextureResource> &getSkybox() const { return mSkyboxTexture; }
	void setSkybox(const ResourceReference<TextureResource> &skyboxTexture) { mSkyboxTexture = skyboxTexture; }

protected:
	Console &mConsole;
	ResourceManager &mResourceManager;

	Camera mCamera;

	ResourceReference<TextureResource> mSkyboxTexture;

private:
	Renderer3D(const Renderer3D &);
	Renderer3D &operator=(const Renderer3D &);
};

#endif // __Renderer3D_h__