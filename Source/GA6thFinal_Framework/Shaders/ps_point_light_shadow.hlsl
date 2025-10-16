#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 worldPosition : TEXCOORD1;
    float3 lightPosition : TEXCOORD2;
    float farPlane : TEXCOORD3;
    
    nointerpolation uint4 materialID : TEXCOORD4;
};

Texture2D textures[];
#define DIFFUSE 0

float ps_main(PSInput input) : SV_Depth
{
    uint diffuseID = input.materialID[DIFFUSE];
    
    float alpha = textures[diffuseID].Sample(samLinear_wrap, input.uv).a;
    clip(alpha - CUTOFF);
    float distance = length(input.worldPosition - input.lightPosition);
    float normalizedDepth = distance / input.farPlane;
    
    return normalizedDepth;
}