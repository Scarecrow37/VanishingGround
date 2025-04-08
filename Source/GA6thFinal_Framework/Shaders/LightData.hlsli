#ifndef __LightData__
#define __LightData__

struct Light
{
    float4 diffuse;
    float4 specular;
    float4 ambient;
    float3 data;
    float range;
    float3 attenuation;
    float intensity;
};

cbuffer OptionalDesc : register(b0)
{
    float texel;
    float3 __padding__;
}

cbuffer CameraDesc : register(b1)
{
    matrix viewprojInv;
    float3 cameraPosition;
}

cbuffer DirectionalLights : register(b2)
{
    Light DirectionalLights[8];
}

cbuffer PointLights : register(b3)
{
    Light PointLights[64];
}

cbuffer NumLight : register(b4)
{
    uint numDirectionalLights;
    uint numPointLights;
}

Texture2D   txShadowDepth    : register(t10);
TextureCube txIBL_Diffuse    : register(t11);
TextureCube txIBL_Specular   : register(t12);
Texture2D   txIBL_BRDF       : register(t13);

SamplerState samLinear_wrap          : register(s0);
SamplerState samLinear_clamp         : register(s1);
SamplerComparisonState samComparison : register(s2);

#endif