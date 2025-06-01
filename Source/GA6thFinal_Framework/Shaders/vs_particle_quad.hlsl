#include "Compute.hlsli"
StructuredBuffer<ParticleOutput> particleInfo;


struct VSInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    uint InstanceID : SV_InstanceID; // 인스턴스 ID 추가

};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    float4 color : COLOR0;
    uint emitterIndex : TEXCOORD1; 

};

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    ParticleOutput instanceInfo = particleInfo[input.InstanceID];
    float4 pos = float4(input.position.xyz, 1.f);
    output.position = mul(pos, instanceInfo.FinalMatrix);
    
    output.color = instanceInfo.Color;
    output.emitterIndex = instanceInfo.EmitterIndex;
    
    output.uv = input.uv;
    return output;
}