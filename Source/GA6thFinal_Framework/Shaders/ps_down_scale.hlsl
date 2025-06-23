#include "CommonData.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
};

Texture2D brightExtractTexture;

float4 ps_main(PS_INPUT input) : SV_TARGET
{   
    return brightExtractTexture.Sample(samLinear_wrap, input.uv);
}