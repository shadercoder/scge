#ifndef FINALPASS_HLSL
#define FINALPASS_HLSL

#include "DirectX11SharedDefines.h"
#include "DirectX11Utility.hlsl"
#include "DirectX11Rendering.hlsl"

// --------------------------------------------------------------------------------------
// Globals
// --------------------------------------------------------------------------------------
cbuffer FrameConstants : register( b0 )
{
	float4x4 gProjection;
	float4x4 gInverseView;
	float    gNearPlane;
	float    gFarPlane;
	uint2    gFrameBufferDimensions;
	float3   gEyePosition;
	uint     gNumLights;
};

StructuredBuffer<uint2>				gLitBuffer		: register( t0 );
Texture2DMS<float, MSAA_SAMPLES>	gDepthTexture	: register( t1 );
TextureCube<float4>					gSkyboxTexture	: register( t2 );

SamplerState gSampler : register( s0 );

// --------------------------------------------------------------------------------------
// Vertex Shader
// --------------------------------------------------------------------------------------
float4 VSMain(float2 position : POSITION) : SV_POSITION
{
    return float4(position, 1.0f, 1.0f);
}

float4 PSColour(float4 position : SV_POSITION) : SV_TARGET
{
	uint2 coord = position.xy;

	float3 lit = float3(0.0f, 0.0f, 0.0f);

	[unroll]
	for(uint sampleIndex = 0; sampleIndex < MSAA_SAMPLES; ++sampleIndex)
	{
		float depth = gDepthTexture.Load(coord, sampleIndex).x;

		if(depth <= 0.0f)
		{
			lit += float3(0.0f, 0.0f, 0.0f);
		}
		else
		{
			uint index = GetLitBufferSampleAddress(coord, sampleIndex, gFrameBufferDimensions);
			lit += UnpackRGBA16(gLitBuffer[index]).xyz;
		}
	}

	return float4(lit * rcp(MSAA_SAMPLES), 1.0f);
}

float4 PSSkybox(float4 position : SV_POSITION) : SV_TARGET
{
	uint2 coord = position.xy;

	float3 lit = float3(0.0f, 0.0f, 0.0f);
	uint skyboxSamples = 0;

	[unroll]
	for(uint sampleIndex = 0; sampleIndex < MSAA_SAMPLES; ++sampleIndex)
	{
		float depth = gDepthTexture.Load(coord, sampleIndex).x;

		if(depth <= 0.0f)
		{
			++skyboxSamples;
		}
		else
		{
			uint index = GetLitBufferSampleAddress(coord, sampleIndex, gFrameBufferDimensions);
			lit += UnpackRGBA16(gLitBuffer[index]).xyz;
		}
	}

	[branch]
	if(skyboxSamples > 0)
	{
		float2 screenPixelOffset = float2(2.0f, -2.0f) / float2(gFrameBufferDimensions);
		float2 positionScreen = position.xy + 0.5f;
		positionScreen *= screenPixelOffset;
		positionScreen += float2(-1.0f, 1.0f);

		float viewSpaceZ = gProjection._43 / -gProjection._33;
		
		float3 positionView = ComputeViewPositionFromZ(positionScreen, viewSpaceZ, gProjection);

		float3 eyeDir = mul(float4(normalize(positionView), 0.0f), gInverseView).xyz;

		float3 skybox = gSkyboxTexture.SampleLevel(gSampler, eyeDir, 0).xyz;

		lit += skyboxSamples * skybox;
	}

	return float4(lit * rcp(MSAA_SAMPLES), 1.0f);
}

#endif // FINALPASS_HLSL