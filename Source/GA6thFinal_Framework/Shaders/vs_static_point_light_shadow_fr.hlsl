#include "CommonData.hlsli"

struct VSInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    float2 lightUV : TEXCOORD1;
    
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
};

struct ShadowMeshData
{
    uint FaceIndex;
    uint Offset;
};

ConstantBuffer<ShadowMeshData> bit32_2_shadowMeshData;
ConstantBuffer<PointLightShadowData> pointLightShadowData;

VSOutput vs_main(VSInput input)
{
    uint offset = bit32_2_shadowMeshData.Offset;
    uint faceIndex = bit32_2_shadowMeshData.FaceIndex;
    InstanceData data = instanceData[input.instanceID + offset];
    
    VSOutput output = (VSOutput) 0;
    
    float4 worldPos = mul(input.position, matrices[data.MatrixID].World);
    output.position = worldPos;
    
    output.position = mul(worldPos, pointLightShadowData.ViewProjection[faceIndex]);
    output.lightPosition = pointLightShadowData.LightPosition;
    output.farPlane = pointLightShadowData.FarPlane;
    
    output.uv = input.uv;
    output.materialID = data.MaterialID;
    return output;
}
