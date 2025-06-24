#include "CommonData.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
};

Texture2D sourceTexture;
RWTexture2D<float4> accumulation;

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    //float indices[KERNEL_SIZE] = { -4, -3, -2, -1, 0, +1, +2, +3, +4 };
    float indices[KERNEL_SIZE] = { -7, -6, -5, -4, -3, -2, -1, 0, +1, +2, +3, +4, +5, +6, +7 };
    float2 step = float2(0, 1) * postProcessData.TexelSize;
    float4 result = 0.0f;
    
    [unroll]
    for (int i = 0; i < KERNEL_SIZE; i++)
    {
        result += sourceTexture.Sample(samLinear_clamp, float2(input.uv + indices[i] * step)) * GaussianWeight[i];
    }
    
    accumulation[(uint2)input.position.xy] += float4(result.rgb, 1);

    return float4(0.f, 0.f, 0.f, 0.f);
}