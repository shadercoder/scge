#ifndef COMPUTELIGHTING_HLSL
#define COMPUTELIGHTING_HLSL

#include "DirectX11SharedDefines.h"
#include "DirectX11Utility.hlsl"
#include "DirectX11Rendering.hlsl"

// --------------------------------------------------------------------------------------
// Globals
// --------------------------------------------------------------------------------------
RWStructuredBuffer<uint2> gLitBuffer : register( u0 );
Texture2DMS<float4, MSAA_SAMPLES> gGBuffer[3] : register( t0 );
StructuredBuffer<PointLight> gLights : register( t3 );

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

// --------------------------------------------------------------------------------------
// Group Shared Data
// --------------------------------------------------------------------------------------
groupshared uint sMinZ;
groupshared uint sMaxZ;

groupshared uint sTileLightIndices[MAX_LIGHTS];
groupshared uint sTileNumLights;

groupshared uint sPerSamplePixels[GROUP_SIZE];
groupshared uint sNumPerSamplePixels;

// --------------------------------------------------------------------------------------
// Functions
// --------------------------------------------------------------------------------------
void WriteSample(uint2 coords, uint sampleIndex, float4 value)
{
    gLitBuffer[GetLitBufferSampleAddress(coords, sampleIndex, gFrameBufferDimensions)] = PackRGBA16(value);
}

void WriteAllSamples(uint2 coords, float4 value)
{
	[unroll]
	for(uint sample = 0; sample < MSAA_SAMPLES; ++sample)
	{
		WriteSample(coords, sample, value);
	}
}

void ComputeSurfaceDataFromGBuffer(uint2 viewPosition, uint sampleIndex, out SurfaceData Output)
{
	float4 albedo         = gGBuffer[0].Load(viewPosition, sampleIndex).xyzw;
	float4 normalSpecular = gGBuffer[1].Load(viewPosition, sampleIndex).xyzw;
	float  zBuffer        = gGBuffer[2].Load(viewPosition, sampleIndex).x;

	Output.mAlbedo.rgb = albedo.rgb;
	Output.mAlbedo.a = 1.0f;

	float2 screenPixelOffset = float2(2.0f, -2.0f) / float2(gFrameBufferDimensions);
	float2 positionScreen = float2(viewPosition) + 0.5f;
	positionScreen *= screenPixelOffset;
	positionScreen += float2(-1.0f, 1.0f);

	float viewSpaceZ = gProjection._43 / (zBuffer - gProjection._33);

	Output.mPositionView = ComputeViewPositionFromZ(positionScreen, viewSpaceZ, gProjection);

	//Output.mNormal = DecodeNormal(normalSpecular.xy);
	//Output.mNormal = float3(normalSpecular.xy, -1);
	//Output.mSpecularAmount = normalSpecular.z;
	//Output.mSpecularPower = normalSpecular.w;

	Output.mNormal = normalSpecular.xyz;
	Output.mSpecularAmount = normalSpecular.w;
	Output.mSpecularPower = 25.0f;

	Output.mIsEdge = albedo.a > 0.5f;
}

void ComputeSurfaceDataFromGBufferAllSamples(uint2 positionViewport, out SurfaceData surfaces[MSAA_SAMPLES])
{
	[unroll]
	for(uint i = 0; i < MSAA_SAMPLES; ++i)
	{
		ComputeSurfaceDataFromGBuffer(positionViewport, i, surfaces[i]);
	}
}

float3 LightSample(uint2 sampleCoords, uint sampleIndex, SurfaceData surface)
{
	float3 lit = 0.0f;
	for(uint tileLightIndex = 0; tileLightIndex < sTileNumLights; ++tileLightIndex)
	{
		PointLight light = gLights[sTileLightIndices[tileLightIndex]];
		AccumulateBRDF(surface, light, lit);
	}

	return lit;
}

// --------------------------------------------------------------------------------------
// Compute Shader
// --------------------------------------------------------------------------------------
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void CSMain(uint3 groupId : SV_GroupID, uint3 groupThreadId : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchThreadId : SV_DispatchThreadID)
{
	SurfaceData surfaceSamples[MSAA_SAMPLES];
	ComputeSurfaceDataFromGBufferAllSamples(dispatchThreadId.xy, surfaceSamples);

	float minZSample = surfaceSamples[0].mPositionView.z;
	float maxZSample = surfaceSamples[0].mPositionView.z;

	[unroll]
	for(uint sample = 1; sample < MSAA_SAMPLES; ++sample)
	{
		float viewSpaceZ = surfaceSamples[sample].mPositionView.z;
		minZSample = min(minZSample, viewSpaceZ);
		maxZSample = max(maxZSample, viewSpaceZ);
	}

	if(groupIndex == 0)
	{
		sTileNumLights = 0;
		sNumPerSamplePixels = 0;
		sMinZ = asuint(gFarPlane);
		sMaxZ = asuint(gFarPlane);
	}

	GroupMemoryBarrierWithGroupSync();

	InterlockedMin(sMinZ, asuint(minZSample));
	InterlockedMax(sMaxZ, asuint(maxZSample));

	GroupMemoryBarrierWithGroupSync();

	float minTileZ = asfloat(sMinZ);
	float maxTileZ = asfloat(sMaxZ);

	float2 tileScale = float2(gFrameBufferDimensions) * rcp(float(2 * BLOCK_SIZE));
	float2 tileBias = tileScale - float2(groupId.xy);

	float4 c1 = float4(gProjection._11 * tileScale.x, 0.0f, tileBias.x, 0.0f);
	float4 c2 = float4(0.0f, -gProjection._22 * tileScale.y, tileBias.y, 0.0f);
	float4 c4 = float4(0.0f, 0.0f, 1.0f, 0.0f);

	float4 frustumPlanes[6];
	frustumPlanes[0] = c4 - c1;
	frustumPlanes[1] = c4 + c1;
	frustumPlanes[2] = c4 - c2;
	frustumPlanes[3] = c4 + c2;
	frustumPlanes[4] = float4(0.0f, 0.0f,  1.0f, -minTileZ);
	frustumPlanes[5] = float4(0.0f, 0.0f, -1.0f,  maxTileZ);

	[unroll]
	for(uint i = 0; i < 4; ++i)
	{
		frustumPlanes[i] *= rcp(length(frustumPlanes[i].xyz));
	}

	for(uint lightIndex = groupIndex; lightIndex < gNumLights; lightIndex += GROUP_SIZE)
	{
		PointLight light = gLights[lightIndex];

		bool inFrustum = true;
		[unroll]
		for(uint i = 0; i < 6; ++i)
		{
			float d = dot(frustumPlanes[i], float4(light.mPositionView, 1.0f));
			inFrustum = inFrustum && (d >= -light.mAttenuationEnd);
		}

		[branch]
		if(inFrustum)
		{
			uint listIndex;
			InterlockedAdd(sTileNumLights, 1, listIndex);
			sTileLightIndices[listIndex] = lightIndex;
		}
	}

	GroupMemoryBarrierWithGroupSync();

	if(all(dispatchThreadId.xy < gFrameBufferDimensions.xy))
	{
		float3 lit = LightSample(dispatchThreadId.xy, 0, surfaceSamples[0]);
		
		[branch]
		if(sTileNumLights > 0 && RequiresPerSampleShading(surfaceSamples))
		{
			WriteSample(dispatchThreadId.xy, 0, float4(lit, 1.0f));

			uint listIndex;
			InterlockedAdd(sNumPerSamplePixels, 1, listIndex);
			sPerSamplePixels[listIndex] = PackCoords(dispatchThreadId.xy);
		}
		else
		{
			WriteAllSamples(dispatchThreadId.xy, float4(lit, 1.0f));
		}
	}

#if MSAA_SAMPLES > 1
	GroupMemoryBarrierWithGroupSync();

	const uint shadingPassesPerPixel = MSAA_SAMPLES - 1;
	uint globalSamples = sNumPerSamplePixels * shadingPassesPerPixel;

	for(uint globalSample = groupIndex; globalSample < globalSamples; globalSample += GROUP_SIZE)
	{
		uint listIndex = globalSample / shadingPassesPerPixel;
		uint sampleIndex = globalSample % shadingPassesPerPixel + 1;

		uint2 sampleCoords = UnpackCoords(sPerSamplePixels[listIndex]);

		SurfaceData surface;
		ComputeSurfaceDataFromGBuffer(sampleCoords, sampleIndex, surface);

		float3 lit = LightSample(sampleCoords, sampleIndex, surface);
		WriteSample(sampleCoords, sampleIndex, float4(lit, 1.0f));
	}
#endif
}

#endif // COMPUTELIGHTING_HLSL