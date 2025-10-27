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
    
    nointerpolation uint4 materialID : TEXCOORD2;
};

struct ShadowMeshData
{
    uint Offset;
    uint InstanceCount;
};

ConstantBuffer<ShadowMeshData> bit32_2_shadowMeshData;

VSOutput vs_main(VSInput input)
{
    uint offset = bit32_2_shadowMeshData.Offset;
    uint meshInstanceID = input.instanceID;
   
    InstanceData data = instanceData[meshInstanceID + offset];
    
    VSOutput output = (VSOutput) 0;
    
    float4 worldPos = mul(input.position, matrices[data.MatrixID].World);
    output.worldPosition = worldPos.xyz;
    
    output.position = worldPos;
    
    output.uv = input.uv;
    output.materialID = data.MaterialID;
    return output;
}
