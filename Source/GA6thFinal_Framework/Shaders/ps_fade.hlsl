#include "CommonData.hlsli"

struct timestep
{
    float4 color;
};


ConstantBuffer<timestep> bit32_4_time;

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 ps_main(PSInput input) : SV_Target
{
    return bit32_4_time.color;

}
