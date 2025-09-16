#include "CommonData.hlsli"

struct VSInput
{
    float4 position     : POSITION;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 biTangent    : BINORMAL;
    float2 uv           : TEXCOORD;
    float2 lightUV      : TEXCOORD1;
};

struct VSOutput
{
    float4 position     : SV_POSITION;
    float2 uv           : TEXCOORD;
};

StructuredBuffer<MatrixData> matrices;

VSOutput vs_main(VSInput input)
{
    ShadowObjectData data = bit32_4_shadowObjectData;
    
    VSOutput output = (VSOutput) 0;

    uint instanceID = data.ID;
    uint cascadeIndex = data.CascadeIndex;

    output.position = mul(input.position, matrices[instanceID].World);
    output.position = mul(output.position, cascadeData.ShadowVP[cascadeIndex]);

    output.uv = input.uv;

    return output;
}