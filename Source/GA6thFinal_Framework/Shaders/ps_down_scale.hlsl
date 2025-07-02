#include "CommonData.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D brightExtractTexture;

float4 ps_main(PS_INPUT input) : SV_TARGET
{   
    return brightExtractTexture.Sample(samLinear_wrap, input.uv);
}