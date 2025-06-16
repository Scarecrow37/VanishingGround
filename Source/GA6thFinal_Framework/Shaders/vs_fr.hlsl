#include "CommonData.hlsli"

StructuredBuffer<matrix> worldMatrices;

struct VSInput
{
    float4 position     : POSITION;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 biTangent    : BINORMAL;
    float2 uv           : TEXCOORD;
};

struct VSOutput
{
    float4 position     : SV_POSITION;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 biTangent    : BINORMAL;
    float2 uv           : TEXCOORD;
    float4 worldPosition : POSITION;
};

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    float3 normal = normalize(input.normal);
    float3 tangent = normalize(input.tangent);
    float3 biTangent = normalize(input.biTangent);
    
    output.position = mul(input.position, worldMatrices[objectData.ID]);
    
    output.worldPosition = output.position;
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);
    
    output.normal = normalize(mul(normal, (float3x3) worldMatrices[objectData.ID]));
    output.tangent = normalize(mul(tangent, (float3x3) worldMatrices[objectData.ID]));
    output.biTangent = normalize(mul(biTangent, (float3x3) worldMatrices[objectData.ID]));
    
    output.uv = input.uv;

    return output;
}