#include "CommonData.hlsli"

Texture2D screenTexture : register(t0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 ps_main(PSInput input) : SV_Target
{
    return screenTexture.Sample(samLinear_wrap, input.uv);
}