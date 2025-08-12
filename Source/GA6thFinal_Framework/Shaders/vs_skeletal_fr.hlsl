#include "CommonData.hlsli"


struct VSInput
{
    float4 position      : POSITION;
    float3 normal        : NORMAL;
    float3 tangent       : TANGENT;
    float3 biTangent     : BINORMAL;
    float2 uv            : TEXCOORD;
    float2 lightUV       : TEXCOORD1;
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
    float4 worldPosition : TEXCOORD1;
};

StructuredBuffer<matrix> worldMatrices;
StructuredBuffer<matrix> boneMatrices;

VSOutput vs_main(VSInput input)
{    
    matrix boneTransform = mul(input.blendWeights.x, boneMatrices[objectData.ID * objectData.Offset + input.blendIndices.x]);
    boneTransform += mul(input.blendWeights.y, boneMatrices[objectData.ID * objectData.Offset + input.blendIndices.y]);
    boneTransform += mul(input.blendWeights.z, boneMatrices[objectData.ID * objectData.Offset + input.blendIndices.z]);
    boneTransform += mul(input.blendWeights.w, boneMatrices[objectData.ID * objectData.Offset + input.blendIndices.w]);
    matrix worldTransform = mul(boneTransform, worldMatrices[objectData.ID]);
    
    VSOutput output = (VSOutput) 0;
    
    output.position = mul(input.position, worldTransform);   
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);

    output.normal = normalize(mul(float4(input.normal, 0), worldTransform));
    output.tangent = normalize(mul(float4(input.tangent.xyz, 0), worldTransform));
    output.biTangent = normalize(mul(float4(input.biTangent, 0), worldTransform));
    
    output.uv = input.uv;

    return output;
}