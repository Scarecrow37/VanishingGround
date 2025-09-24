#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct MipLevel
{
    uint Level;
};

ConstantBuffer<MipLevel> bit32_1_mipLevel;
Texture2D sourceTexture;

float4 ps_main(PSInput input) : SV_TARGET
{   
    return sourceTexture.SampleLevel(samLinear_clamp, input.uv, bit32_1_mipLevel.Level);
}