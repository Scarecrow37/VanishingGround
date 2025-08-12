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
};

StructuredBuffer<MatrixData> matrices;

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    
    output.position = mul(input.position, matrices[objectData.ID].World);
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);       
    
    output.normal = normalize(mul(input.normal, (float3x3) matrices[objectData.ID].InverseTranspose));
    output.tangent = normalize(mul(input.tangent, (float3x3) matrices[objectData.ID].InverseTranspose));
    output.biTangent = normalize(mul(input.biTangent, (float3x3) matrices[objectData.ID].InverseTranspose));
    
    output.uv = input.uv;

    return output;
}