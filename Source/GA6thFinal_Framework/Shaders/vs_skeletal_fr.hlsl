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

StructuredBuffer<MatrixData> matrices;
StructuredBuffer<matrix> boneMatrices;

VSOutput vs_main(VSInput input)
{
    ObjectData data = bit32_4_objectData;
    
    matrix boneTransform = mul(input.blendWeights.x, boneMatrices[data.ID * data.Offset + input.blendIndices.x]);
    boneTransform += mul(input.blendWeights.y, boneMatrices[data.ID * data.Offset + input.blendIndices.y]);
    boneTransform += mul(input.blendWeights.z, boneMatrices[data.ID * data.Offset + input.blendIndices.z]);
    boneTransform += mul(input.blendWeights.w, boneMatrices[data.ID * data.Offset + input.blendIndices.w]);
    
    matrix worldTransform = mul(boneTransform, matrices[data.ID].World);
    matrix inverseTranspose = mul(boneTransform, matrices[data.ID].InverseTranspose);
    
    VSOutput output = (VSOutput) 0;
    
    output.position = mul(input.position, worldTransform);   
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);

    output.normal = normalize(mul(input.normal, (float3x3) inverseTranspose));
    output.tangent = normalize(mul(input.tangent, (float3x3) inverseTranspose));
    output.biTangent = normalize(mul(input.biTangent, (float3x3) inverseTranspose));
    
    output.uv = input.uv;

    return output;
}