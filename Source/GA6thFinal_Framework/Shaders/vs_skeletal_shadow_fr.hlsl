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
    
    uint instanceID : SV_InstanceID;
};

struct VSOutput
{
    float4 position      : SV_POSITION;
    float2 uv            : TEXCOORD;
    
    nointerpolation uint4 materialID : TEXCOORD1;
    uint arrayIndex : SV_RenderTargetArrayIndex;
};

struct ShadowMeshData
{
    uint InstanceCount;
    uint Offset;
};

ConstantBuffer<ShadowMeshData> bit32_2_shadowMeshData;
StructuredBuffer<uint> meshData;
StructuredBuffer<matrix> boneMatrices;

VSOutput vs_main(VSInput input)
{
    uint offset = bit32_2_shadowMeshData.Offset;
    uint instanceCount = bit32_2_shadowMeshData.InstanceCount;
    uint cascadeIndex = input.instanceID / instanceCount;
    InstanceData data = instanceData[input.instanceID % instanceCount + offset];

    matrix boneTransform = mul(input.blendWeights.x, boneMatrices[data.MatrixID * MAX_BONE_MATRIX + input.blendIndices.x]);
    boneTransform       += mul(input.blendWeights.y, boneMatrices[data.MatrixID * MAX_BONE_MATRIX + input.blendIndices.y]);
    boneTransform       += mul(input.blendWeights.z, boneMatrices[data.MatrixID * MAX_BONE_MATRIX + input.blendIndices.z]);
    boneTransform       += mul(input.blendWeights.w, boneMatrices[data.MatrixID * MAX_BONE_MATRIX + input.blendIndices.w]);

    matrix worldTransform = mul(boneTransform, matrices[data.MatrixID].World);
    
    VSOutput output = (VSOutput) 0;
    
    output.position = mul(input.position, worldTransform);
    output.position = mul(output.position, cascadeData.ShadowVP[cascadeIndex]);

    output.uv = input.uv;
    output.materialID = data.MaterialID;
    output.arrayIndex = cascadeIndex;
    
    return output;
}