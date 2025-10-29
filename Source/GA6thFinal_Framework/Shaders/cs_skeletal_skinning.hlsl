#include "CommonData.hlsli"

struct SkinnedVertex
{
    float4 position;
    float3 normal;
    float3 tangent;
    float3 biTangent;
    float2 uv[2];
};

struct Vertex
{
    float4 position;
    float3 normal;
    float3 tangent;
    float3 biTangent;
    float2 uv;
    float2 lightUV;
    uint4 blendIndices;
    float4 blendWeights;
};

struct MeshData
{
    uint instanceID;
    uint boneMatrixCount;
    uint vertexCount;
};

RWStructuredBuffer<SkinnedVertex> skinnedVertices;
StructuredBuffer<Vertex> vertices;
StructuredBuffer<matrix> boneMatrices;
ConstantBuffer<MeshData> bit32_3_meshData;

[numthreads(256, 1, 1)]
void cs_main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint vertexIndex = dispatchThreadID.x;
    
    if (vertexIndex >= bit32_3_meshData.vertexCount)
        return;
    
    Vertex input = vertices[vertexIndex];
    uint boneOffset = bit32_3_meshData.instanceID * bit32_3_meshData.boneMatrixCount;
    matrix boneTransform = mul(input.blendWeights.x, boneMatrices[boneOffset + input.blendIndices.x]);
    boneTransform += mul(input.blendWeights.y, boneMatrices[boneOffset + input.blendIndices.y]);
    boneTransform += mul(input.blendWeights.z, boneMatrices[boneOffset + input.blendIndices.z]);
    boneTransform += mul(input.blendWeights.w, boneMatrices[boneOffset + input.blendIndices.w]);

    SkinnedVertex output;
    output.position = mul(input.position, boneTransform);
    output.normal = normalize(mul(input.normal, (float3x3) boneTransform));
    output.tangent = normalize(mul(input.tangent, (float3x3) boneTransform));
    output.biTangent = normalize(mul(input.biTangent, (float3x3) boneTransform));
    output.uv[0] = input.uv;
    output.uv[1] = input.lightUV;

    skinnedVertices[vertexIndex] = output;

}