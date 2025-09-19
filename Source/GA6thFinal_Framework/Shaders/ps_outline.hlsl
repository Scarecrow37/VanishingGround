#include "CommonData.hlsli"
#include "Function.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D<float4> depthTexture;
Texture2D<float4> normalTexture;
Texture2D<uint> customDepthTexture;
RWTexture2D<float4> accumulation;

void ps_main(PSInput input)
{   
    PostProcessData data = bit32_6_postProcessData;
    
    float centerDepth = depthTexture.Sample(samPoint_clamp, input.uv).r;
    float3 centerNormal = normalize(normalTexture.Sample(samPoint_clamp, input.uv).xyz); // [-1, 1] 범위로 복원
    
    float depthEdge = 0.0;
    float normalEdge = 0.0;
    
    float2 offsets[4] =
    {
        float2(0, data.TexelSize.y),
        float2(0, -data.TexelSize.y),
        float2(data.TexelSize.x, 0),
        float2(-data.TexelSize.x, 0)
    };
    
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        float2 neighborUV = input.uv + offsets[i];
    
        float neighborDepth = depthTexture.Sample(samPoint_clamp, neighborUV).r;
        float3 neighborNormal = normalize(normalTexture.Sample(samPoint_clamp, neighborUV).xyz);

        depthEdge += abs(centerDepth - neighborDepth);
        normalEdge += 1.0 - saturate(dot(centerNormal, neighborNormal));
    }
        
    float finalDepthEdge = step(0.01, depthEdge);
    float finalNormalEdge = step(0.9, 1.0 - normalEdge);
    
    float outlineFactor = saturate(finalDepthEdge + finalNormalEdge);     

    accumulation[(uint2) input.position.xy] += float4(float3(10, 5, 0) * (1 - outlineFactor.xxx), 1.0) * CalculatePostProcessMask(customDepthTexture, input.uv); // 외곽선은 흰색, 배경은 검정   
}