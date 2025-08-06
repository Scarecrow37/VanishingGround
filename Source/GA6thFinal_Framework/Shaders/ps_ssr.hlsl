#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D screenColor;
Texture2D screenNormal;
Texture2D screenDepth;

float4 ps_main(PSInput input) : SV_Target
{
    float2 uv = input.uv;
    float3 color = screenColor.SampleLevel(samLinear_clamp, uv, 0).xyz;
    float3 normal = screenNormal.SampleLevel(samLinear_clamp, uv, 0).xyz;
    float depth = screenDepth.SampleLevel(samLinear_clamp, uv, 0).r;
    float3 refelctColor = float3(0.0, 0.0, 0.0);
    
    
    
    float4 finalColor = float4(color + refelctColor, 1.f);
    return finalColor;
}