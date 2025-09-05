#include "CommonData.hlsli"

struct timestep
{
    float time;
    float4 start;
    float4 end;
};


ConstantBuffer<timestep> bit32_9_time;

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 ps_main(PSInput input) : SV_Target
{
    float4 result = lerp(bit32_9_time.start, bit32_9_time.end, bit32_9_time.time);
    return result;

}
