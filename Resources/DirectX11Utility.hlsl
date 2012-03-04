#ifndef UTILITY_HLSL
#define UTILITY_HLSL

#include "DirectX11SharedDefines.h"

float2 EncodeNormal(float3 n)
{
	float oneMinusZ = 1.0f - n.z;
	float p = sqrt(n.x * n.x + n.y * n.y + oneMinusZ * oneMinusZ);
	return n.xy / p * 0.5f + 0.5f;
}

float3 DecodeNormal(float2 e)
{
	float2 tmp = e - e * e;
	float f = tmp.x + tmp.y;
	float m = sqrt(4.0f * f - 1.0f);

	float3 n;
	n.xy = m * (e * 4.0f - 2.0f);
	n.z  = 3.0f - 8.0f * f;
	return n;
}

/*float2 EncodeNormal(float3 n)
{
	float f = sqrt(8*n.z+8);
	return n.xy / f + 0.5;
}

float3 DecodeNormal(float2 e)
{
	float2 fenc = e*4-2;
	float f = dot(fenc, fenc);
	float g = sqrt(1-f/4);

	float3 n;
	n.xy = fenc*g;
	n.z = 1-f/2;
	return n;
}*/

float4 UnpackRGBA16(uint2 e)
{
	return float4(f16tof32(e), f16tof32(e >> 16));
}

uint2 PackRGBA16(float4 c)
{
	return f32tof16(c.rg) | (f32tof16(c.ba) << 16);
}

uint PackCoords(uint2 coords)
{
	return coords.y << 16 | coords.x;
}

uint2 UnpackCoords(uint coords)
{
	return uint2(coords & 0xFFFF, coords >> 16);
}

float linstep(float min, float max, float v)
{
	return saturate((v - min) / (max - min));
}

float3 ComputeViewPositionFromZ(float2 positionScreen, float viewSpaceZ, float4x4 projection)
{
	float3 positionView;

	positionView.z = viewSpaceZ;

	float2 screenSpaceRay = float2(positionScreen.x / projection._11, positionScreen.y / projection._22);
	positionView.xy = screenSpaceRay.xy * positionView.z;

	return positionView;
}

float3 ComputeFaceNormal(float3 position)
{
	return cross(ddx_coarse(position), ddy_coarse(position));
}

uint GetLitBufferSampleAddress(uint2 coords, uint sampleIndex, uint2 bufferDimensions)
{
    return (sampleIndex * bufferDimensions.y + coords.y) * bufferDimensions.x + coords.x;
}

#endif // UTILITY_HLSL