#include "scge\Graphics\DirectX11\DirectX11Renderer3D.h"

#include "scge\..\..\Resources\DirectX11SharedDefines.h"
#include "scge\Graphics\PointLight.h"

DirectX11Renderer3D::DirectX11Renderer3D(Console &console, ResourceManager &resourceManager)
	: Renderer3D(console, resourceManager)
	, mDevice(nullptr)
	, mDeviceContext(nullptr)
	, mSwapChain(nullptr)
	, mNumPointLights(0)
{
}

DirectX11Renderer3D::~DirectX11Renderer3D()
{
	SCGE_ASSERT_MESSAGE(mPointLights.empty(), "Lifetime of point lights longer then the Renderer3D they are assigned to");
	auto it = mPointLights.begin(), end = mPointLights.end();
	while(it != end)
		(*(it++))->Release();
}

bool DirectX11Renderer3D::onInitialiseDevice(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
{
	mDevice = device;
	mDeviceContext = deviceContext;

	CD3D11_SAMPLER_DESC samplerDesc(D3D11_DEFAULT);
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxAnisotropy = 16;

	if(FAILED(mDevice->CreateSamplerState(&samplerDesc, mDiffuseSampler.getModifieablePointer())))
		return true;

	CD3D11_BLEND_DESC blendDesc(D3D11_DEFAULT);
	if(FAILED(mDevice->CreateBlendState(&blendDesc, mGeometryBlendState.getModifieablePointer())))
		return true;

	CD3D11_DEPTH_STENCIL_DESC dsDesc(D3D11_DEFAULT);
	dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	if(FAILED(mDevice->CreateDepthStencilState(&dsDesc, mDepthState.getModifieablePointer())))
		return true;

	if(mPerObjectCB.Initialise(mDevice))
		return true;

	if(CreateLightBuffer())
		return true;

	if(CreateFullScreenQuad())
		return true;

	mGBufferVS = mResourceManager.getResourceReference<DirectX11VertexShader>("[VertexShader] DirectX11RenderGBuffer.hlsl VSMain");
	mGBufferOpaquePS = mResourceManager.getResourceReference<DirectX11PixelShader>("[PixelShader] DirectX11RenderGBuffer.hlsl OpaquePSMain");
	mGBufferAlphaTestPS = mResourceManager.getResourceReference<DirectX11PixelShader>("[PixelShader] DirectX11RenderGBuffer.hlsl AlphaTestPSMain");
	mComputeLightingCS = mResourceManager.getResourceReference<DirectX11ComputeShader>("[ComputeShader] DirectX11ComputeLighting.hlsl CSMain");
	mFinalPassVS = mResourceManager.getResourceReference<DirectX11VertexShader>("[VertexShader] DirectX11FinalPass.hlsl VSMain QuadLayout");
	mFinalPassPSColour = mResourceManager.getResourceReference<DirectX11PixelShader>("[PixelShader] DirectX11FinalPass.hlsl PSColour");
	mFinalPassPSSkybox = mResourceManager.getResourceReference<DirectX11PixelShader>("[PixelShader] DirectX11FinalPass.hlsl PSSkybox");

	return false;
}

void DirectX11Renderer3D::onReleaseDevice()
{
	mFinalPassPSSkybox.Reset();
	mFinalPassVS.Reset();
	mComputeLightingCS.Reset();
	mGBufferAlphaTestPS.Reset();
	mGBufferOpaquePS.Reset();
	mGBufferVS.Reset();

	mFullScreenQuadBuffer.Release();

	mLightBuffer.Release();

	mPerObjectCB.Release();

	mDepthState.Release();
	mGeometryBlendState.Release();
	mDiffuseSampler.Release();

	mDevice = nullptr;
	mDeviceContext = nullptr;
}

bool DirectX11Renderer3D::onInitialiseSwapChain(IDXGISwapChain *swapChain, bool windowed, unsigned int width, unsigned int height)
{
	mSwapChain = swapChain;
	mWidth = width;
	mHeight = height;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	if(FAILED(mSwapChain->GetDesc(&swapChainDesc)))
		return true;

	CD3D11_RASTERIZER_DESC rasterStateDesc(D3D11_DEFAULT);
	rasterStateDesc.MultisampleEnable = swapChainDesc.SampleDesc.Count > 1;
	if(FAILED(mDevice->CreateRasterizerState(&rasterStateDesc, mOpaqueRasterizerState.getModifieablePointer())))
		return true;

	rasterStateDesc.CullMode = D3D11_CULL_NONE;
	if(FAILED(mDevice->CreateRasterizerState(&rasterStateDesc, mAlphaTestRasterizerState.getModifieablePointer())))
		return true;

	if(CreateRenderTarget(swapChainDesc))
		return true;

	if(CreateDepthBuffer(swapChainDesc))
		return true;

	if(mPerFrameConstants.Initialise(mDevice))
		return true;

	if(CreateGBuffer(swapChainDesc))
		return true;

	if(CreateLitBuffer(swapChainDesc))
		return true;

	return false;
}

void DirectX11Renderer3D::onReleaseSwapChain()
{
	mLitBuffer.Release();

	mGBufferSRV.clear();
	mGBufferRTV.clear();
	mGBuffer.clear();

	mPerFrameConstants.Release();

	mDepthBufferReadOnlyDSV.Release();
	mDepthBuffer.Release();

	mRenderTargetSRV.Release();
	mRenderTargetView.Release();

	mAlphaTestRasterizerState.Release();
	mOpaqueRasterizerState.Release();
}

bool DirectX11Renderer3D::Render()
{
	SetupViewProjection();

	RenderGBuffer();

	ComputeLighting();

	FinalPass();

	return false;
}

bool DirectX11Renderer3D::CreateRenderTarget(const DXGI_SWAP_CHAIN_DESC &swapChainDesc)
{
	ComPtr<ID3D11Texture2D> backBuffer;
	if(FAILED(mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &backBuffer)))
		return true;

	if(FAILED(mDevice->CreateRenderTargetView(backBuffer, nullptr, mRenderTargetView.getModifieablePointer())))
		return true;

	CD3D11_SHADER_RESOURCE_VIEW_DESC DescSRV(D3D11_SRV_DIMENSION_TEXTURE2DMS, swapChainDesc.BufferDesc.Format);
	if(FAILED(mDevice->CreateShaderResourceView(backBuffer, &DescSRV, mRenderTargetSRV.getModifieablePointer())))
		return true;

	return false;
}

bool DirectX11Renderer3D::CreateDepthBuffer(const DXGI_SWAP_CHAIN_DESC &swapChainDesc)
{
	if(mDepthBuffer.Initialise(mDevice, mWidth, mHeight, swapChainDesc.SampleDesc, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, swapChainDesc.SampleDesc.Count > 1))
		return true;

	D3D11_DEPTH_STENCIL_VIEW_DESC DSVdesc;
	mDepthBuffer.GetDepthStencil()->GetDesc(&DSVdesc);
	DSVdesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;

	if(FAILED(mDevice->CreateDepthStencilView(mDepthBuffer.GetTexture(), &DSVdesc, mDepthBufferReadOnlyDSV.getModifieablePointer())))
		return true;

	return false;
}

bool DirectX11Renderer3D::CreateGBuffer(const DXGI_SWAP_CHAIN_DESC &swapChainDesc)
{
	mGBuffer.resize(2);
	if(mGBuffer[0].Initialise(mDevice, mWidth, mHeight, DXGI_FORMAT_R8G8B8A8_UNORM, swapChainDesc.SampleDesc))
		return true;
	if(mGBuffer[1].Initialise(mDevice, mWidth, mHeight, DXGI_FORMAT_R16G16B16A16_FLOAT, swapChainDesc.SampleDesc))
		return true;

	mGBufferRTV.resize(mGBuffer.size());
	mGBufferSRV.resize(mGBuffer.size() + 1);
	for(size_t i = 0; i < mGBuffer.size(); ++i)
	{
		mGBufferRTV[i] = mGBuffer[i].GetRenderTarget();
		mGBufferSRV[i] = mGBuffer[i].GetShaderResource();
	}

	mGBufferSRV.back() = mDepthBuffer.GetShaderResource();

	return false;
}

bool DirectX11Renderer3D::CreateLitBuffer(const DXGI_SWAP_CHAIN_DESC &swapChainDesc)
{
	if(mLitBuffer.Initialise(mDevice, mWidth * mHeight * swapChainDesc.SampleDesc.Count, D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE, true))
		return true;

	return false;
}

void DirectX11Renderer3D::SetupViewProjection()
{
	mView = mCamera.getViewMatrix();

	mCameraNear = 0.1f;
	mCameraFar = 5000.0f;
	mProjection = Math::CreatePerspectiveProjection(Math::DegreesToRadians(65.0f), (float)mWidth / (float)mHeight, mCameraFar, mCameraNear);

	mViewProjection = mView * mProjection;
}

void DirectX11Renderer3D::RenderGBuffer()
{
	mDeviceContext->ClearDepthStencilView(mDepthBuffer.GetDepthStencil(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, 0);

	mDeviceContext->PSSetSamplers(0, 1, &mDiffuseSampler);
	mDeviceContext->OMSetDepthStencilState(mDepthState, 0);
	mDeviceContext->OMSetRenderTargets(mGBufferRTV.size(), mGBufferRTV.data(), mDepthBuffer.GetDepthStencil());
	mDeviceContext->OMSetBlendState(mGeometryBlendState, 0, 0xFFFFFFFF);

	if(mGBufferVS.isReady())
	{
		mDeviceContext->IASetInputLayout(mGBufferVS->getInputLayout());
		mDeviceContext->VSSetShader(mGBufferVS->getShader(), nullptr, 0);

		if(mGBufferOpaquePS.isReady())
		{
			mDeviceContext->RSSetState(mOpaqueRasterizerState);
			mDeviceContext->PSSetShader(mGBufferOpaquePS->getShader(), nullptr, 0);

			for(auto model : mModelsToRender)
			{
				if(model->getResourceStatus() != ResourceStatus::Ready)
					continue;

				auto pVSPerObject = mPerObjectCB.MapDiscard(mDeviceContext);

				pVSPerObject->mWorldView = mView;
				pVSPerObject->mWorldViewProjection = mViewProjection;

				mPerObjectCB.Unmap(mDeviceContext);

				mDeviceContext->VSSetConstantBuffers(0, 1, mPerObjectCB.GetBufferPointer());
				mDeviceContext->PSSetConstantBuffers(0, 1, mPerObjectCB.GetBufferPointer());

				model->Render(mDeviceContext, false);
			}
		}

		if(mGBufferAlphaTestPS.isReady())
		{
			mDeviceContext->RSSetState(mAlphaTestRasterizerState);
			mDeviceContext->PSSetShader(mGBufferAlphaTestPS->getShader(), nullptr, 0);

			for(auto model : mModelsToRender)
			{
				if(model->getResourceStatus() != ResourceStatus::Ready)
					continue;

				auto pVSPerObject = mPerObjectCB.MapDiscard(mDeviceContext);

				pVSPerObject->mWorldView = mView;
				pVSPerObject->mWorldViewProjection = mViewProjection;

				mPerObjectCB.Unmap(mDeviceContext);

				mDeviceContext->VSSetConstantBuffers(0, 1, mPerObjectCB.GetBufferPointer());
				mDeviceContext->PSSetConstantBuffers(0, 1, mPerObjectCB.GetBufferPointer());

				model->Render(mDeviceContext, true);
			}
		}
	}

	ID3D11RenderTargetView *nullViews[] = { nullptr, nullptr };
	mDeviceContext->OMSetRenderTargets(2, nullViews, nullptr);

	mModelsToRender.clear();
}

void DirectX11Renderer3D::ComputeLighting()
{
	SetupLights();

	auto constants = mPerFrameConstants.MapDiscard(mDeviceContext);

	constants->mProjection = mProjection;
	constants->gInverseView = mView.InversedCopy();
	constants->mNearPlane = mCameraNear;
	constants->mFarPlane = mCameraFar;
	constants->mFrameBufferWidth = mWidth;
	constants->mFrameBufferHeight = mHeight;
	constants->mEyePosition = mCamera.getPosition();
	constants->mNumLights = mNumPointLights;

	mPerFrameConstants.Unmap(mDeviceContext);

	mDeviceContext->CSSetConstantBuffers(0, 1, mPerFrameConstants.GetBufferPointer());
	mDeviceContext->CSSetShaderResources(0, 3, mGBufferSRV.data());
	mDeviceContext->CSSetShaderResources(3, 1, mLightBuffer.GetShaderResourcePointer());
	mDeviceContext->CSSetUnorderedAccessViews(0, 1, mLitBuffer.GetUnorderedAccessPointer(), nullptr);

	if(mComputeLightingCS.isReady())
	{
		mDeviceContext->CSSetShader(mComputeLightingCS->getShader(), 0, 0);

		unsigned int dispatchWidth = (mWidth + BLOCK_SIZE - 1) / BLOCK_SIZE;
		unsigned int dispatchHeight = (mHeight + BLOCK_SIZE - 1) / BLOCK_SIZE;
		mDeviceContext->Dispatch(dispatchWidth, dispatchHeight, 1);
	}

	ID3D11ShaderResourceView *nullSRV[] = { nullptr, nullptr, nullptr, nullptr };
	mDeviceContext->CSSetShaderResources(0, 4, nullSRV);

	ID3D11UnorderedAccessView *nullUAV[] = { nullptr };
	mDeviceContext->CSSetUnorderedAccessViews(0, 1, nullUAV, 0);
}

void DirectX11Renderer3D::FinalPass()
{
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);

	mDeviceContext->PSSetConstantBuffers(0, 1, mPerFrameConstants.GetBufferPointer());

	const static unsigned int stride = sizeof(Vector2);
	const static unsigned int offset = 0;

	mDeviceContext->IASetVertexBuffers(0, 1, mFullScreenQuadBuffer.GetBufferPointer(), &stride, &offset);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	ID3D11ShaderResourceView *LitDepthSRVs[] = { mLitBuffer.GetShaderResource(), mDepthBuffer.GetShaderResource() };
	mDeviceContext->PSSetShaderResources(0, 2, LitDepthSRVs);

	if(mFinalPassVS.isReady())
	{
		mDeviceContext->IASetInputLayout(mFinalPassVS->getInputLayout());
		mDeviceContext->VSSetShader(mFinalPassVS->getShader(), nullptr, 0);

		if(mSkyboxTexture.isReady())
		{
			mDeviceContext->PSSetShaderResources(2, 1, mSkyboxTexture->GetShaderResourceViewPointer());

			if(mFinalPassPSSkybox.isReady())
			{
				mDeviceContext->PSSetShader(mFinalPassPSSkybox->getShader(), nullptr, 0);

				mDeviceContext->Draw(4, 0);
			}
		}
		else
		{
			if(mFinalPassPSColour.isReady())
			{
				mDeviceContext->PSSetShader(mFinalPassPSColour->getShader(), nullptr, 0);

				mDeviceContext->Draw(4, 0);
			}
		}
	}

	ID3D11ShaderResourceView *nullSRV[] = { nullptr, nullptr, nullptr };
	mDeviceContext->PSSetShaderResources(0, 3, nullSRV);
}

bool DirectX11Renderer3D::CreateLightBuffer()
{
	if(mLightBuffer.Initialise(mDevice, MAX_LIGHTS, D3D11_BIND_SHADER_RESOURCE, true, nullptr, true))
		return true;

	return false;
}

void DirectX11Renderer3D::SetupLights()
{
	auto lights = mLightBuffer.MapDiscard(mDeviceContext);

	mNumPointLights = 0;
	for(const PointLight *pointLight : mPointLights)
	{
		SCGE_ASSERT(mNumPointLights < MAX_LIGHTS);
		lights[mNumPointLights].mPositionView = pointLight->getPosition() * mView;
		lights[mNumPointLights].mColour = pointLight->getColour();
		lights[mNumPointLights].mAttenuationBegin = pointLight->getAttenuationBegin();
		lights[mNumPointLights].mAttenuationEnd = pointLight->getAttenuationEnd();
		++mNumPointLights;
	}

	mLightBuffer.Unmap(mDeviceContext);
}

bool DirectX11Renderer3D::CreateFullScreenQuad()
{
	Vector2 vertices[4];
	vertices[0] = Vector2(-1.0f, 1.0f);
	vertices[1] = Vector2(1.0f, 1.0f);
	vertices[2] = Vector2(-1.0f, -1.0f);
	vertices[3] = Vector2(1.0f, -1.0f);

	if(mFullScreenQuadBuffer.Initialise(mDevice, 4, D3D11_BIND_VERTEX_BUFFER, false, vertices))
		return true;

	return false;
}

void DirectX11Renderer3D::registerPointLight(PointLight &pointLight)
{
	mPointLights.insert(&pointLight);
}

void DirectX11Renderer3D::unRegisterPointLight(PointLight &pointLight)
{
	mPointLights.erase(&pointLight);
}

void DirectX11Renderer3D::addModelToQueue(const DirectX11Model* model)
{
	mModelsToRender.push_back(model);
}