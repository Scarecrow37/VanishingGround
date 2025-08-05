#include "CommonData.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D<float4> sourceTexture;
RWTexture2D<float4> outputTexture;

void ps_main(PS_INPUT input)
{    
    //float indices[KERNEL_SIZE] = { -7, -6, -5, -4, -3, -2, -1, 0, +1, +2, +3, +4, +5, +6, +7 };
    float indices[9] = {  -4, -3, -2, -1, 0, +1, +2, +3, +4};
    float2 step = float2(0, 1) * postProcessData.TexelSize;
    float4 result = 0.0f;
    
    [unroll]
    for (int i = 0; i < 9; i++)
    {
        outputTexture[(int2)input.position.xy] += sourceTexture.Sample(samLinear_clamp, float2(input.uv + indices[i] * step)) * GaussianWeight[i];
    }
}