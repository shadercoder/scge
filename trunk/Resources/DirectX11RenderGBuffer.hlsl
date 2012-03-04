#ifndef RENDERGBUFFER_HLSL
#define RENDERGBUFFER_HLSL

#include "DirectX11SharedDefines.h"
#include "DirectX11Utility.hlsl"
#include "DirectX11Rendering.hlsl"

// --------------------------------------------------------------------------------------
// Globals
// --------------------------------------------------------------------------------------
cbuffer PerObjectConstants : register( b0 )
{
	float4x4 gWorldViewProjection   : packoffset( c0 );
	float4x4 gWorldView             : packoffset( c4 );
};

// --------------------------------------------------------------------------------------
// Textures and Samplers
// --------------------------------------------------------------------------------------
Texture2D    gDiffuse  : register( t0 );
Texture2D    gNormal   : register( t1 );
Texture2D    gSpecular : register( t2 );
SamplerState gSampler  : register( s0 );

// --------------------------------------------------------------------------------------
// Structures
// --------------------------------------------------------------------------------------
struct VS_INPUT
{
	float3 mPosition : POSITION;
	float2 mTexCoord : TEXCOORD;
	float3 mNormal   : NORMAL;
	float3 mBinormal : BINORMAL;
	float3 mTangent  : TANGENT;
};

struct VS_OUTPUT
{
	float4   mPosition      : SV_POSITION;
	float2   mTexCoord      : TEXCOORD0;
	float3   mPositionView  : TEXCOORD1;
	float3x3 mTangentToView : TEXCOORD2;
};

struct PS_INPUT
{
	float4   mPosition      : SV_POSITION;
	float2   mTexCoord      : TEXCOORD0;
	float3   mPositionView  : TEXCOORD1;
	float3x3 mTangentToView : TEXCOORD2;
	uint     mCoverage      : SV_COVERAGE;
};

struct PS_OUTPUT
{
	float4 mAlbedo         : SV_TARGET0;
	float4 mNormalSpecular : SV_TARGET1;
};

// --------------------------------------------------------------------------------------
// Functions
// --------------------------------------------------------------------------------------
SurfaceData ComputeSurfaceDataFromGeometry(PS_INPUT Input)
{
	SurfaceData Output;

	Output.mPositionView = Input.mPositionView;

	float3 normalFromMap = gNormal.Sample(gSampler, Input.mTexCoord).rgb;
	normalFromMap = (2.0f * normalFromMap) - 1.0f;
	normalFromMap = mul(normalFromMap, Input.mTangentToView);
	Output.mNormal = normalize(normalFromMap);
	//Output.mNormal = normalize(ComputeFaceNormal(Input.mPositionView));
	//Output.mNormal = Input.mTangentToView[2];

	Output.mAlbedo = gDiffuse.Sample(gSampler, Input.mTexCoord);

	float3 specular = gSpecular.Sample(gSampler, Input.mTexCoord).rgb;
	Output.mSpecularAmount = (specular.r + specular.g + specular.b) / 3;
	Output.mSpecularPower = 10.0f;

	Output.mIsEdge = Input.mCoverage != uint(pow(2, MSAA_SAMPLES) - 1) && Input.mCoverage != 0;

	return Output;
}

// --------------------------------------------------------------------------------------
// Vertex Shader
// --------------------------------------------------------------------------------------
VS_OUTPUT VSMain(VS_INPUT Input)
{
	VS_OUTPUT Output;
	
	Output.mPosition     = mul(float4(Input.mPosition, 1.0f), gWorldViewProjection);
	Output.mPositionView = mul(float4(Input.mPosition, 1.0f), gWorldView).xyz;

	Output.mTexCoord = Input.mTexCoord;

	Output.mTangentToView[0] = mul(float4(Input.mTangent, 0.0f), gWorldView).xyz;
	Output.mTangentToView[1] = mul(float4(Input.mBinormal, 0.0f), gWorldView).xyz;
	Output.mTangentToView[2] = mul(float4(Input.mNormal, 0.0f), gWorldView).xyz;

	return Output;
}

// --------------------------------------------------------------------------------------
// Pixel Shader
// --------------------------------------------------------------------------------------
PS_OUTPUT OpaquePSMain(PS_INPUT Input)
{
	PS_OUTPUT Output;

	SurfaceData surfaceData = ComputeSurfaceDataFromGeometry(Input);

	Output.mAlbedo.rgb = surfaceData.mAlbedo.rgb;
	Output.mAlbedo.a = surfaceData.mIsEdge ? 1.0f : 0.0f;

	//Output.mNormalSpecular = float4(surfaceData.mNormal.xy, surfaceData.mSpecularAmount, surfaceData.mSpecularPower);
	Output.mNormalSpecular = float4(surfaceData.mNormal, surfaceData.mSpecularAmount);
	//Output.mNormalSpecular = float4(EncodeNormal(surfaceData.mNormal), surfaceData.mSpecularAmount, surfaceData.mSpecularPower);
	
	return Output;
}

PS_OUTPUT AlphaTestPSMain(PS_INPUT Input)
{
	PS_OUTPUT Output;

	SurfaceData surfaceData = ComputeSurfaceDataFromGeometry(Input);

	clip(surfaceData.mAlbedo.a - 0.3f);

	Output.mAlbedo.rgb = surfaceData.mAlbedo.rgb;
	Output.mAlbedo.a = surfaceData.mIsEdge ? 1.0f : 0.0f;

	float3 faceNormal = normalize(ComputeFaceNormal(Input.mPositionView));

	float3 normalToUse = (dot(faceNormal, surfaceData.mNormal) < 0.0f) ? -surfaceData.mNormal : surfaceData.mNormal;

	Output.mNormalSpecular = float4(normalToUse, surfaceData.mSpecularAmount);

	//Output.mNormalSpecular = float4(surfaceData.mNormal.xy, surfaceData.mSpecularAmount, surfaceData.mSpecularPower);
	//Output.mNormalSpecular = float4(surfaceData.mNormal, surfaceData.mSpecularAmount);
	//Output.mNormalSpecular = float4(EncodeNormal(surfaceData.mNormal), surfaceData.mSpecularAmount, surfaceData.mSpecularPower);
	
	return Output;
}

#endif // RENDERGBUFFER_HLSL