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
    float3 normal        : NORMAL;
    float3 tangent       : TANGENT;
    float3 biTangent     : BINORMAL;
    float2 uv            : TEXCOORD;
    float4 worldPosition : TEXCOORD1;
    
    nointerpolation uint4 materialID : TEXCOORD2;
    nointerpolation uint  customDepth : TEXCOORD3;
    nointerpolation float alpha : TEXCOORD4;
};

struct Offset
{
    uint Offset;
};

ConstantBuffer<Offset> bit32_1_offset;
StructuredBuffer<matrix> boneMatrices;

VSOutput vs_main(VSInput input)
{
    uint offset = bit32_1_offset.Offset;
    InstanceData data = instanceData[input.instanceID + offset];

    matrix boneTransform  = mul(input.blendWeights.x, boneMatrices[data.MatrixID * MAX_BONE_MATRIX + input.blendIndices.x]);
           boneTransform += mul(input.blendWeights.y, boneMatrices[data.MatrixID * MAX_BONE_MATRIX + input.blendIndices.y]);
           boneTransform += mul(input.blendWeights.z, boneMatrices[data.MatrixID * MAX_BONE_MATRIX + input.blendIndices.z]);
           boneTransform += mul(input.blendWeights.w, boneMatrices[data.MatrixID * MAX_BONE_MATRIX + input.blendIndices.w]);
    
    matrix worldTransform = mul(boneTransform, matrices[data.MatrixID].World);
    matrix inverseTranspose = mul(boneTransform, matrices[data.MatrixID].InverseTranspose);
    
    VSOutput output = (VSOutput) 0;
    
    output.position = mul(input.position, worldTransform);
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);

    output.normal = normalize(mul(input.normal, (float3x3) inverseTranspose));
    output.tangent = normalize(mul(input.tangent, (float3x3) inverseTranspose));
    output.biTangent = normalize(mul(input.biTangent, (float3x3) inverseTranspose));
    
    output.uv = input.uv;
    output.materialID = data.MaterialID;
    output.customDepth = data.CustomDepth;
    output.alpha = data.Alpha;
    
    return output;
}