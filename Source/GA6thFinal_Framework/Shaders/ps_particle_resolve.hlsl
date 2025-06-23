// ps_particle_resolve.hlsl
#include "CommonData.hlsli"
//struct Resolution
//{
//    uint Width;
//    uint Height;
//};
//ConstantBuffer<Resolution> bit32_2_resolution;


Texture2D<float4> gAccumTex;
Texture2D<float> gRevealTex;

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
};

float4 ps_main(PSInput input) : SV_Target
{
    float2 uv = input.uv;

    float4 accum = gAccumTex.Sample(samLinear_border, uv);
    float reveal = gRevealTex.Sample(samLinear_border, uv).r;
    float3 color = accum.rgb / max(accum.a, 1e-6);
    reveal = reveal * accum.a * accum.a;
    return float4(color, reveal);

}
