#include "CommonData.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D sourceTexture;

float4 ps_main(PS_INPUT input) : SV_TARGET
{    
    //float indices[KERNEL_SIZE] = { -7, -6, -5, -4, -3, -2, -1, 0, +1, +2, +3, +4, +5, +6, +7 };
    float indices[9] = {  -4, -3, -2, -1, 0, +1, +2, +3, +4};
    float2 step = float2(1, 0) * postProcessData.TexelSize;
    float4 result = 0.0f;
    
    [unroll]
    for (int i = 0; i < 9; i++)
    {
        result += sourceTexture.SampleLevel(samLinear_clamp, float2(input.uv + indices[i] * step), postProcessData.MipLevel) * GaussianWeight[i];
    }
    
    return float4(result.rgb, 1);
}