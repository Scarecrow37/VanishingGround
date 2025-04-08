#include "ShaderUtilities.hlsli"

struct PS_INPUT
{
    float4 position         : SV_POSITION;
    float2 uv               : TEXCOORD;
};

Texture2D txSource0         : register(t2);
Texture2D txSource1         : register(t3);

SamplerState samLinear_wrap : register(s0);

float4 main(PS_INPUT input) : SV_Target
{
    float4 t0 = txSource0.Sample(samLinear_wrap, input.uv);
    float4 t1 = txSource1.Sample(samLinear_wrap, input.uv);
    
    float4 color = t0 + t1;
    //float4 color = t0 * t1 + t0;
    
    color.a = 1;
    return color;
}