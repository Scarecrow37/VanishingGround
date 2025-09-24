#include "CommonData.hlsli"

struct VSInput
{
    float4 position     : POSITION;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 biTangent    : BINORMAL;
    float2 uv           : TEXCOORD;
    float2 lightUV      : TEXCOORD1;
    
    uint instanceID : SV_InstanceID;
};

struct VSOutput
{
    float4 position     : SV_POSITION;
    float2 uv           : TEXCOORD;
    
    nointerpolation uint4 materialID : TEXCOORD1;
};

struct ShadowMeshData
{
    uint Offset;
    uint CascadeIndex;
};

ConstantBuffer<ShadowMeshData> bit32_2_shadowMeshData;

VSOutput vs_main(VSInput input)
{
    uint offset = bit32_2_shadowMeshData.Offset;
    uint cascadeIndex = bit32_2_shadowMeshData.CascadeIndex;
    InstanceData data = instanceData[input.instanceID + offset];
    
    VSOutput output = (VSOutput) 0;

    output.position = mul(input.position, matrices[data.MatrixID].World);
    output.position = mul(output.position, cascadeData.ShadowVP[cascadeIndex]);

    output.uv = input.uv;
    output.materialID = data.MaterialID;

    return output;
}