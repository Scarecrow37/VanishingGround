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
    float4 position      : SV_POSITION;
    float3 normal        : NORMAL;
    float3 tangent       : TANGENT;
    float3 biTangent     : BINORMAL;
    float2 uv            : TEXCOORD;
    float4 worldPosition : TEXCOORD1;
};

StructuredBuffer<MatrixData> matrices;

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    
    TranslucentObjectData data = bit32_4_translucentObjectData;
    
    output.position = mul(input.position, matrices[data.ID].World);
    output.worldPosition = output.position;
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);       
    
    output.normal = normalize(mul(input.normal, (float3x3) matrices[data.ID].InverseTranspose));
    output.tangent = normalize(mul(input.tangent, (float3x3) matrices[data.ID].InverseTranspose));
    output.biTangent = normalize(mul(input.biTangent, (float3x3) matrices[data.ID].InverseTranspose));
    
    output.uv = input.uv;

    return output;
}