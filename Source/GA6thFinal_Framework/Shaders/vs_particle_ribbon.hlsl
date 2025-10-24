// vs_particle_ribbon.hlsl
#include "Compute.hlsli"
#include "CommonData.hlsli"

StructuredBuffer<ParticleOutput> particleInfo;

struct DrawParams
{
    uint EmitStart;
};
ConstantBuffer<DrawParams> bit32_1_emitterOffset; // 루트상수 1개

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR0;
    float depth : TEXCOORD1;
    nointerpolation int emitterIndex : CUSTOM_FLAG;
};

VSOutput vs_main(uint vertexId : SV_VertexID)
{
    VSOutput o = (VSOutput) 0;

    // 간접 인덱스 제거: ribbonIndices[vertexId] 대신
    uint vertexIndex = bit32_1_emitterOffset.EmitStart + vertexId;
    ParticleOutput p = particleInfo[vertexIndex];

    o.position = p.Position; // 이미 clip 공간이면 그대로, view/proj가 필요하면 기존대로 곱하세요
    float u = p.FrameInfo.z;
    float v = (float) (vertexIndex & 1); // 0=Top, 1=Bottom
    o.uv = float2(u, v);

    o.color = p.Color;
    o.depth = o.position.z / o.position.w;
    o.emitterIndex = p.EmitterIndex;
    return o;
}