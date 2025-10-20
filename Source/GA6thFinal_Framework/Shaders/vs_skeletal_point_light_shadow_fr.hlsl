#include "CommonData.hlsli"

struct VSInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    float2 lightUV : TEXCOORD1;
    uint4 blendIndices : BLENDINDICES;
    float4 blendWeights : BLENDWEIGHT;
    
    uint instanceID : SV_InstanceID;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 worldPosition : TEXCOORD1;
    float3 lightPosition : TEXCOORD2;
    float farPlane : TEXCOORD3;
    
    nointerpolation uint4 materialID : TEXCOORD4;
    uint viewport : SV_ViewportArrayIndex;
};

struct ShadowMeshData
{
    uint Offset;
    uint InstanceCount;
};

ConstantBuffer<ShadowMeshData> bit32_2_shadowMeshData;
ConstantBuffer<PointLightShadowData> pointLightShadowData;
StructuredBuffer<uint> meshData;
StructuredBuffer<matrix> boneMatrices;

VSOutput vs_main(VSInput input)
{
    uint offset = bit32_2_shadowMeshData.Offset;
    uint instanceCount = bit32_2_shadowMeshData.InstanceCount;
    uint meshInstanceID = input.instanceID % instanceCount;
    uint faceIndex = input.instanceID / instanceCount;
    InstanceData data = instanceData[meshInstanceID + offset];
    
    matrix boneTransform = mul(input.blendWeights.x, boneMatrices[data.MatrixID * MAX_BONE_MATRIX + input.blendIndices.x]);
    boneTransform += mul(input.blendWeights.y, boneMatrices[data.MatrixID * MAX_BONE_MATRIX + input.blendIndices.y]);
    boneTransform += mul(input.blendWeights.z, boneMatrices[data.MatrixID * MAX_BONE_MATRIX + input.blendIndices.z]);
    boneTransform += mul(input.blendWeights.w, boneMatrices[data.MatrixID * MAX_BONE_MATRIX + input.blendIndices.w]);

    matrix worldTransform = mul(boneTransform, matrices[data.MatrixID].World);
    
    VSOutput output = (VSOutput) 0;
    
    float4 worldPos = mul(input.position, worldTransform);
    output.worldPosition = worldPos.xyz;
    
    output.position = mul(worldPos, pointLightShadowData.ViewProjection[faceIndex]);
    output.lightPosition = pointLightShadowData.LightPosition;
    output.farPlane = pointLightShadowData.FarPlane;
    
    output.uv = input.uv;
    output.materialID = data.MaterialID;
    
    output.viewport = faceIndex;
    return output;
}
