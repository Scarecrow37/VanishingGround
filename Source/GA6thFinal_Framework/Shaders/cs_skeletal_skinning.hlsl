#include "CommonData.hlsli"

struct SkinnedVertex
{
    float4 position;
    float3 normal;
    float3 tangent;
    float3 biTangent;
    float2 uv[2];
};

RWStructuredBuffer<SkinnedVertex> skinnedVertices : register(u0);

[numthreads(256, 1, 1)]
void cs_main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
}
