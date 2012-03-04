#include "scge\Graphics\DirectX11\DirectX11Renderer2D.h"

DirectX11Renderer2D::DirectX11Renderer2D(Console &console, ResourceManager &resourceManager)
	: Renderer2D(console, resourceManager)
	, mDevice(nullptr)
	, mDeviceContext(nullptr)
	, mSwapChain(nullptr)
{
}

DirectX11Renderer2D::~DirectX11Renderer2D()
{
}

bool DirectX11Renderer2D::onInitialiseDevice(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
{
	mDevice = device;
	mDeviceContext = deviceContext;

	return false;
}

void DirectX11Renderer2D::onReleaseDevice()
{
	mDevice = nullptr;
	mDeviceContext = nullptr;
}

bool DirectX11Renderer2D::onInitialiseSwapChain(IDXGISwapChain *swapChain, bool windowed, unsigned int width, unsigned int height)
{
	mSwapChain = swapChain;

	return false;
}

void DirectX11Renderer2D::onReleaseSwapChain()
{
	mSwapChain = nullptr;
}

bool DirectX11Renderer2D::Render()
{
	return false;
}