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
    float2 uv            : TEXCOORD;
};

StructuredBuffer<MatrixData> matrices;
StructuredBuffer<matrix> boneMatrices;

VSOutput vs_main(VSInput input)
{
    ShadowObjectData data = bit32_4_shadowObjectData;
    
    uint instanceID = data.ID;
    uint cascadeIndex = data.CascadeIndex;

    matrix boneTransform = mul(input.blendWeights.x, boneMatrices[instanceID * data.Offset + input.blendIndices.x]);
    boneTransform       += mul(input.blendWeights.y, boneMatrices[instanceID * data.Offset + input.blendIndices.y]);
    boneTransform       += mul(input.blendWeights.z, boneMatrices[instanceID * data.Offset + input.blendIndices.z]);
    boneTransform       += mul(input.blendWeights.w, boneMatrices[instanceID * data.Offset + input.blendIndices.w]);

    matrix worldTransform = mul(boneTransform, matrices[instanceID].World);
    
    VSOutput output = (VSOutput) 0;
    
    output.position = mul(input.position, worldTransform);
    output.position = mul(output.position, cascadeData.ShadowVP[cascadeIndex]);

    output.uv = input.uv;

    return output;
}