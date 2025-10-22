#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

#define GAUSSIAN_TAB 5

Texture2D sourceTexture;

float4 ps_main(PSInput input) : SV_TARGET
{    
    PostProcessData data = bit32_6_postProcessData;
    
    // 5-tab
    float indices[5] = { -2, -1, 0, +1, +2 };
    // 9-tab
    // float indices[9] = {  -4, -3, -2, -1, 0, +1, +2, +3, +4};
    float2 step = float2(0, 1) * data.TexelSize;
    float4 result = 0.0f;
    
    [unroll]
    for (int i = 0; i < GAUSSIAN_TAB; i++)
    {
        result += sourceTexture.SampleLevel(samLinear_clamp, float2(input.uv + indices[i] * step), data.MipLevel) * GaussianWeight[i];
    }
    
    return float4(result.rgb, 1);
}