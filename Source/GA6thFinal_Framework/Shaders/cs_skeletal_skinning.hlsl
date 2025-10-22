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

RWStructuredBuffer<SkinnedVertex> skinnedVertices;
StructuredBuffer<Vertex> vertices;
StructuredBuffer<matrix> boneMatrices;

[numthreads(256, 1, 1)]
void cs_main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    //MeshData data = bit32_4_objectData;
    
    //uint vertexIndex = dispatchThreadID.x;
    //// custom depth is used to objects vertex count
    //if (vertexIndex >= data.CustomDepth)
    //    return;
    //Vertex input = vertices[vertexIndex];
    //matrix boneTransform = mul(input.blendWeights.x, boneMatrices[data.ID * data.Offset + input.blendIndices.x]);
    //boneTransform += mul(input.blendWeights.y, boneMatrices[data.ID * data.Offset + input.blendIndices.y]);
    //boneTransform += mul(input.blendWeights.z, boneMatrices[data.ID * data.Offset + input.blendIndices.z]);
    //boneTransform += mul(input.blendWeights.w, boneMatrices[data.ID * data.Offset + input.blendIndices.w]);

    //SkinnedVertex output;
    //output.position = mul(input.position, boneTransform);
    
    //output.normal = normalize(mul(input.normal, (float3x3) boneTransform));
    //output.tangent = normalize(mul(input.tangent, (float3x3) boneTransform));
    //output.biTangent = normalize(mul(input.biTangent, (float3x3) boneTransform));
    ////output.position = input.position;
    ////output.normal = normalize(input.normal);
    ////output.tangent = normalize(input.tangent);
    ////output.biTangent = normalize(input.biTangent);
    //output.uv[0] = input.uv;
    //output.uv[1] = input.lightUV;

    //skinnedVertices[vertexIndex] = output;
}
