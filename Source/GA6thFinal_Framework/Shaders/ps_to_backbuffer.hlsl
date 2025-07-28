#include "CommonData.hlsli"

struct PSInput
{
    float4 position     : SV_POSITION;
    float2 uv           : TEXCOORD;
};

Texture2D screenTexture;

float4 ps_main(PSInput input) : SV_TARGET
{
    float4 color = screenTexture.Sample(samLinear_wrap, input.uv);
    
    return color;
}