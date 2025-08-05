#include "CommonData.hlsli"
#include "Function.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct MipLevel
{
    uint LowLevel;
    uint HighLevel;
};

ConstantBuffer<MipLevel> bit32_2_mipLevel;
Texture2D lowTexture;
Texture2D highTexture;

float4 ps_main(PS_INPUT input) : SV_TARGET
{    
    float3 lowColor = lowTexture.SampleLevel(samLinear_clamp, input.uv, bit32_2_mipLevel.LowLevel).rgb;
    float3 highColor = highTexture.SampleLevel(samLinear_clamp, input.uv, bit32_2_mipLevel.HighLevel).rgb;
    
    return float4(lowColor + highColor, 1.f);
}