#include "CommonData.hlsli"
#include "Function.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D<float4> worldTexture;
Texture2D<uint> customDepthTexture;
RWTexture2D<float4> accumulation;

void ps_main(PS_INPUT input)
{   
    float3 world = worldTexture.Sample(samLinear_wrap, input.uv).xyz;
    float3 viewPosition = mul((float3x3) cameraData.View, world);
    
    float edge = 0.0;    
    float viewDepthThreshold = 1;
    
    [unroll]
    for (int dist = 1; dist <= 2; ++dist)
    {
        float2 offsets[8] =
        {
            float2(postProcessData.TexelSize.x, 0) * dist,
            float2(-postProcessData.TexelSize.x, 0) * dist,
            float2(0, postProcessData.TexelSize.y) * dist,
            float2(0, -postProcessData.TexelSize.y) * dist,
            float2(postProcessData.TexelSize.x, postProcessData.TexelSize.y) * dist,
            float2(-postProcessData.TexelSize.x, postProcessData.TexelSize.y) * dist,
            float2(postProcessData.TexelSize.x, -postProcessData.TexelSize.y) * dist,
            float2(-postProcessData.TexelSize.x, -postProcessData.TexelSize.y) * dist
        };

        [unroll]
        for (int i = 0; i < 8; ++i)
        {
            float2 neighborUV = input.uv + offsets[i];

            float3 neighborWorld = worldTexture.Sample(samLinear_wrap, neighborUV);
            float3 neighborViewPos = mul(cameraData.View, float4(neighborWorld, 1.0)).xyz;

            float depthDiff = abs(viewPosition.z - neighborViewPos.z);

            if (depthDiff > viewDepthThreshold)
            {
                edge = 1.0;
                break;
            }
        }

        if (edge > 0.0)
            break;
    }

    accumulation[(uint2) input.position.xy] += float4(float3(10, 5, 0) * edge.xxx, 1.0) * CalculatePostProcessMask(customDepthTexture, input.uv); // 외곽선은 흰색, 배경은 검정   
}