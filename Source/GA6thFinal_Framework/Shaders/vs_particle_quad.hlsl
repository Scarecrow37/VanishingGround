#include "Compute.hlsli"

StructuredBuffer<ParticleOutput> particleInfo;

struct VSInput
{
    float4 position : POSITION;
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
    nointerpolation uint2 emitterIndex : CUSTOM_FLAG;
};

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    ParticleOutput instanceInfo = particleInfo[input.InstanceID];
    
    float4 pos = float4(input.position.xyz, 1.f);
    
    output.position = mul(pos, instanceInfo.FinalMatrix);
    
    
    output.color = instanceInfo.Color;
    output.emitterIndex.x = instanceInfo.EmitterIndex;
    output.emitterIndex.y = (int) instanceInfo.Paddings2.x;

    if(instanceInfo.FrameInfo.x < 0)
    {
        output.uv = input.uv;
    }
    else
    {
        output.uv = float2(
        lerp(instanceInfo.FrameInfo.x, instanceInfo.FrameInfo.y, input.uv.x),
        lerp(instanceInfo.FrameInfo.z, instanceInfo.FrameInfo.w, input.uv.y));
    }
    return output;
}