#include "CommonData.hlsli"
#include "Compute.hlsli"
struct RibbonVertexCount
{
    uint count;
};
//ConstantBuffer<RibbonVertexCount> bit32_1_ribbonVertexCount;

StructuredBuffer<uint> ribbonIndices;

StructuredBuffer<ParticleOutput> particleInfo;

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

VSOutput vs_main(uint vertexId : SV_VertexID)
{
    VSOutput o = (VSOutput) 0;

    uint currentIdx = vertexId / 2;
    uint particleIndex = ribbonIndices[currentIdx];
    ParticleOutput p = particleInfo[particleIndex];
    float3 posCurr = p.Position.xyz;
    float3 offsetVector = normalize(p.Paddings);
    int isTop = (vertexId % 2 == 0) ? 1 : -1;
    float ribbonHalfWidth = p.FrameInfo.x * 0.5f;

    float3 finalWorldPos = posCurr + (offsetVector * ribbonHalfWidth * isTop);

    float4 viewPos = mul(float4(finalWorldPos, 1.0f), cameraData.View);
    o.position = mul(viewPos, cameraData.Projection);

    // UV 및 나머지 데이터
    float u = p.FrameInfo.z;
    float v = (float) (vertexId % 2);
    o.uv = float2(u, v);

    o.color = p.Color;
    o.emitterIndex.x = p.EmitterIndex;
    o.emitterIndex.y = p.Paddings2.x;
    return o;
}