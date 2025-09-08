#include "CommonData.hlsli"

struct timestep
{
    float4 start;
    float4 end;
    float time;
};


ConstantBuffer<timestep> bit32_9_time;

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 ps_main(PSInput input) : SV_Target
{
    float r1 = bit32_9_time.start.r;
    float g1 = bit32_9_time.start.g;
    float b1 = bit32_9_time.start.b;
    float a1 = bit32_9_time.start.a;
    float r2 = bit32_9_time.end.r;
    float g2 = bit32_9_time.end.g;
    float b2 = bit32_9_time.end.b;
    float a2 = bit32_9_time.end.a;
    float t = bit32_9_time.time;
    
    float r = lerp(r1, r2, t);
    float g = lerp(g1, g2, t);
    float b = lerp(b1, b2, t);
    float a = lerp(a1, a2, t);
    return float4(r, g, b, a);

}
