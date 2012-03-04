#ifndef __DirectX11Renderer3D_h__
#define __DirectX11Renderer3D_h__

#include "scge\Graphics\DirectX11\DirectX11Renderer.h"

#include "scge\Graphics\Renderer3D.h"
#include "scge\Graphics\DirectX11\DirectX11ComputeShader.h"
#include "scge\Graphics\DirectX11\DirectX11VertexShader.h"
#include "scge\Graphics\DirectX11\DirectX11PixelShader.h"
#include "scge\Graphics\DirectX11\DirectX11Model.h"

#include "scge\Graphics\DirectX11\DirectX11Depth2DHelper.h"
#include "scge\Graphics\DirectX11\DirectX11Texture2DHelper.h"
#include "scge\Graphics\DirectX11\DirectX11BufferHelper.h"

class DirectX11Renderer3D : public Renderer3D
{
public:
	DirectX11Renderer3D(Console &console, ResourceManager &resourceManager);
	virtual ~DirectX11Renderer3D();

	bool onInitialiseDevice(ID3D11Device *device, ID3D11DeviceContext *deviceContext);
	void onReleaseDevice();

	bool onInitialiseSwapChain(IDXGISwapChain *swapChain, bool windowed, unsigned int width, unsigned int height);
	void onReleaseSwapChain();

	bool Render();

private:
	ID3D11Device *mDevice;
	ID3D11DeviceContext *mDeviceContext;
	IDXGISwapChain *mSwapChain;

	unsigned int mWidth;
	unsigned int mHeight;
	
	CComPtr<ID3D11SamplerState> mDiffuseSampler;
	CComPtr<ID3D11BlendState> mGeometryBlendState;

	CComPtr<ID3D11RasterizerState> mOpaqueRasterizerState;
	CComPtr<ID3D11RasterizerState> mAlphaTestRasterizerState;

	ResourceReference<DirectX11VertexShader> mGBufferVS;
	ResourceReference<DirectX11PixelShader> mGBufferOpaquePS;
	ResourceReference<DirectX11PixelShader> mGBufferAlphaTestPS;

	ResourceReference<DirectX11ComputeShader> mComputeLightingCS;

	ResourceReference<DirectX11VertexShader> mFinalPassVS;
	ResourceReference<DirectX11PixelShader> mFinalPassPSColour;
	ResourceReference<DirectX11PixelShader> mFinalPassPSSkybox;

	std::vector<ResourceReference<DirectX11Model>> mOpaqueModels;
	std::vector<ResourceReference<DirectX11Model>> mAlphaTestModels;

	void SetupViewProjection();
	Matrix4 mView;
	Matrix4 mProjection;
	Matrix4 mViewProjection;
	float mCameraNear;
	float mCameraFar;

	struct PerObjectCB
	{
		Matrix4 mWorldViewProjection;
		Matrix4 mWorldView;
	};
	DirectX11BufferHelper<PerObjectCB> mPerObjectCB;

	bool CreateRenderTarget(const DXGI_SWAP_CHAIN_DESC &swapChainDesc);
	CComPtr<ID3D11RenderTargetView> mRenderTargetView;
	CComPtr<ID3D11ShaderResourceView> mRenderTargetSRV;

	bool CreateDepthBuffer(const DXGI_SWAP_CHAIN_DESC &swapChainDesc);
	DirectX11Depth2DHelper mDepthBuffer;
	CComPtr<ID3D11DepthStencilView> mDepthBufferReadOnlyDSV;
	CComPtr<ID3D11DepthStencilState> mDepthState;

	bool CreateGBuffer(const DXGI_SWAP_CHAIN_DESC &swapChainDesc);
	std::vector<DirectX11Texture2DHelper> mGBuffer;
	std::vector<ID3D11RenderTargetView*> mGBufferRTV;
	std::vector<ID3D11ShaderResourceView*> mGBufferSRV;

	bool CreateLitBuffer(const DXGI_SWAP_CHAIN_DESC &swapChainDesc);
	struct FramebufferFlatElement
	{
		unsigned int rb;
		unsigned int ga;
	};
	DirectX11BufferHelper<FramebufferFlatElement> mLitBuffer;

	bool CreateLightBuffer();
	void SetupLights();
	struct PointLight
	{
		Vector3 mPositionView;
		float mAttenuationBegin;
		Vector3 mColour;
		float mAttenuationEnd;
	};
	DirectX11BufferHelper<PointLight> mLightBuffer;
	std::vector<PointLight> mLightProperties;

	struct PerFrameConstants
	{
		Matrix4 mProjection;
		Matrix4 gInverseView;

		float mNearPlane;
		float mFarPlane;

		unsigned int mFrameBufferWidth;
		unsigned int mFrameBufferHeight;

		Vector3 mEyePosition;

		unsigned int mNumLights;
	};
	DirectX11BufferHelper<PerFrameConstants> mPerFrameConstants;

	bool CreateFullScreenQuad();
	DirectX11BufferHelper<Vector2> mFullScreenQuadBuffer;

	void RenderGBuffer();
	void ComputeLighting();
	void FinalPass();
};

#endif // __DirectX11Renderer3D_h__