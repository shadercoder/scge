#ifndef __DirectX11Renderer2D_h__
#define __DirectX11Renderer2D_h__

#include "scge\Graphics\DirectX11\DirectX11Renderer.h"

#include "scge\Graphics\Renderer2D.h"

class DirectX11Renderer2D : public Renderer2D
{
public:
	DirectX11Renderer2D(Console &console, ResourceManager &resourceManager);
	virtual ~DirectX11Renderer2D();

	bool onInitialiseDevice(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
	void onReleaseDevice();

	bool onInitialiseSwapChain(IDXGISwapChain *swapChain, bool windowed, unsigned int width, unsigned int height);
	void onReleaseSwapChain();

	bool Render();

private:
	ID3D11Device *mDevice;
	ID3D11DeviceContext *mDeviceContext;
	IDXGISwapChain *mSwapChain;
};

#endif // __DirectX11Renderer2D_h__