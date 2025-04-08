#include "PostProcess.hlsli"

Texture2D txSource1 : register(t2);

cbuffer SamplingDesc : register(b1)
{
    uint level;
};

SamplerState samLinear_clamp : register(s1);

float4 main(PS_INPUT input) : SV_Target
{
    float4 color = 0;   
    
    color += txSource1.SampleLevel(samLinear_clamp, input.uv, 1) * 0.4;
    color += txSource1.SampleLevel(samLinear_clamp, input.uv, 2) * 0.3;
    color += txSource1.SampleLevel(samLinear_clamp, input.uv, 3) * 0.2;
    color += txSource1.SampleLevel(samLinear_clamp, input.uv, 4) * 0.05;
    color += txSource1.SampleLevel(samLinear_clamp, input.uv, 5) * 0.05;
    
    return float4(color.rgb, 1.f);
}