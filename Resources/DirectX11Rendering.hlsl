#ifndef RENDERING_HLSL
#define RENDERING_HLSL

#include "DirectX11SharedDefines.h"
#include "DirectX11Utility.hlsl"

struct SurfaceData
{
	float3 mPositionView;
	float3 mNormal;
	float4 mAlbedo;
	float  mSpecularAmount;
	float  mSpecularPower;
	bool   mIsEdge;
};

bool RequiresPerSampleShading(SurfaceData surfaceSamples[MSAA_SAMPLES])
{
	bool isEdge = false;

	[unroll]
	for(uint i = 0; i < MSAA_SAMPLES; ++i)
	{
		isEdge = isEdge || surfaceSamples[i].mIsEdge;
	}

	return isEdge;
}

struct PointLight
{
	float3 mPositionView;
	float  mAttenuationBegin;
	float3 mColour;
	float  mAttenuationEnd;
};

void AccumulatePhongBRDF(float3 normal, float3 lightDir, float3 viewDir, float3 lightContrib, float specularPower, inout float3 litDiffuse, inout float3 litSpecular)
{
    float NdotL = dot(normal, lightDir);

    [flatten]
	if(NdotL > 0.0f)
	{
		float3 r = reflect(lightDir, normal);
		float RdotV = max(0.0f, dot(r, viewDir));
		float specular = pow(RdotV, specularPower);

		litDiffuse += lightContrib * NdotL;
		litSpecular += lightContrib * specular;
    }
}

void AccumulateBRDF(SurfaceData surface, PointLight light, inout float3 lit)
{
	float3 directionToLight = light.mPositionView - surface.mPositionView;
	float distanceToLight = length(directionToLight);

	[branch]
	if(distanceToLight < light.mAttenuationEnd)
	{
		float attenuation = linstep(light.mAttenuationEnd, light.mAttenuationBegin, distanceToLight);
		directionToLight *= rcp(distanceToLight);

		float3 litDiffuse = float3(0.0f, 0.0f, 0.0f);
		float3 litSpecular = float3(0.0f, 0.0f, 0.0f);
		AccumulatePhongBRDF(surface.mNormal, directionToLight, normalize(surface.mPositionView), attenuation * light.mColour, surface.mSpecularPower, litDiffuse, litSpecular);
        
		lit += surface.mAlbedo.rgb * (litDiffuse + surface.mSpecularAmount * litSpecular);
	}
}

#endif // RENDERING_HLSL