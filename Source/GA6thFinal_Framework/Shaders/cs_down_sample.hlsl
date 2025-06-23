#include "CommonData.hlsli"

Texture2D InputTex : register(t0);
RWTexture2D OutputTex : register(u0);
cbuffer CB
{
    float2 TexelSize;
};

[numthreads(8, 8, 1)]
void cs_main(uint3 id : SV_DispatchThreadID)
{
    float2 uv = (id.xy + 0.5) * TexelSize;
    float4 sum = float4(0, 0, 0, 0);
    
    // 2x2 박스 필터
    sum += InputTex.SampleLevel(samLinear_wrap, uv + float2(-0.5, -0.5) * TexelSize, 0);
    sum += InputTex.SampleLevel(samLinear_wrap, uv + float2(0.5, -0.5) * TexelSize, 0);
    sum += InputTex.SampleLevel(samLinear_wrap, uv + float2(-0.5, 0.5) * TexelSize, 0);
    sum += InputTex.SampleLevel(samLinear_wrap, uv + float2(0.5, 0.5) * TexelSize, 0);
    
    OutputTex[id.xy] = sum * 0.25;
}