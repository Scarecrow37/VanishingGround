#include "CommonData.hlsli"

Texture2D<float4> gAccumTex;
Texture2D<float> gRevealTex;

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 ps_main(PSInput input) : SV_Target
{
    float2 uv = input.uv;

    float4 accum = gAccumTex.Sample(samLinear_border, uv);
    float reveal = gRevealTex.Sample(samLinear_border, uv).r;
    float3 color = accum.rgb / max(saturate(accum.a), 1e-6);
    return float4(color, saturate(reveal));

}
