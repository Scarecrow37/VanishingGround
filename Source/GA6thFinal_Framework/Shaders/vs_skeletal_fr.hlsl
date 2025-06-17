#include "CommonData.hlsli"


struct VSInput
{
    float4 position      : POSITION;
    float3 normal        : NORMAL;
    float3 tangent       : TANGENT;
    float3 biTangent     : BINORMAL;
    float2 uv            : TEXCOORD;
    uint4  blendIndices  : BLENDINDICES;
    float4 blendWeights  : BLENDWEIGHT;
};

struct VSOutput
{
    float4 position      : SV_POSITION;
    float3 normal        : NORMAL;
    float3 tangent       : TANGENT;
    float3 biTangent     : BINORMAL;
    float2 uv            : TEXCOORD;
    float4 worldPosition : POSITION;
};

struct BoneTransform
{
    matrix Transform;
};

StructuredBuffer<matrix> worldMatrices;
StructuredBuffer<BoneTransform> boneMatrices;

VSOutput vs_main(VSInput input)
{    
    matrix boneTransform = mul(input.blendWeights.x, boneMatrices[objectData.ID * objectData.Offset + input.blendIndices.x].Transform);
    boneTransform += mul(input.blendWeights.y, boneMatrices[objectData.ID * objectData.Offset + input.blendIndices.y].Transform);
    boneTransform += mul(input.blendWeights.z, boneMatrices[objectData.ID * objectData.Offset + input.blendIndices.z].Transform);
    boneTransform += mul(input.blendWeights.w, boneMatrices[objectData.ID * objectData.Offset + input.blendIndices.w].Transform);
    matrix worldTransform = mul(boneTransform, worldMatrices[objectData.ID]);
    
    VSOutput output = (VSOutput) 0;
    float3 normal = normalize(input.normal);
    float3 tangent = normalize(input.tangent);
    float3 biTangent = normalize(input.biTangent);
    
    output.position = mul(input.position, worldTransform);
    
    output.worldPosition = output.position;
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);
    
    output.normal = normalize(mul(normal, (float3x3) worldTransform));
    output.tangent = normalize(mul(tangent, (float3x3) worldTransform));
    output.biTangent = normalize(mul(biTangent, (float3x3) worldTransform));
    
    output.uv = input.uv;

    return output;
}