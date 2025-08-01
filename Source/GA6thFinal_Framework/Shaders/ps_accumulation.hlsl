#include "CommonData.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D sourceTexture;
RWTexture2D<float4> accumulation;

void ps_main(PS_INPUT input)
{
    accumulation[(uint2) input.position.xy] += float4(sourceTexture.Sample(samLinear_clamp, input.uv).rgb, 1);
}