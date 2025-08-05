#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D AOTexture;
Texture2D LightTexture;

float4 ps_main(PSInput input) : SV_Target
{
    float4 pixelColor = LightTexture.SampleLevel(samLinear_clamp, input.uv, 0);
    float AO = AOTexture.SampleLevel(samLinear_clamp, input.uv, 0).r;
    float4 finialColor = float4(pixelColor.xyz * AO,1.f);
    return finialColor;
};