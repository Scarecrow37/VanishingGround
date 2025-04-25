#include "CommonData.hlsli"

ConstantBuffer<Object> bit32_object : register(b1);
StructuredBuffer<ObjectData> objectData;

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

#define object bit32_object

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    float3 normal = normalize(input.normal);
    float3 tangent = normalize(input.tangent);
    float3 biTangent = normalize(input.biTangent);
    
    output.position = mul(input.position, objectData[object.ID].World);
    
    output.worldPosition = output.position;
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);
    
    output.normal = normalize(mul(normal, (float3x3) objectData[object.ID].World));
    output.tangent = normalize(mul(tangent, (float3x3) objectData[object.ID].World));
    output.biTangent = normalize(mul(biTangent, (float3x3) objectData[object.ID].World));
    
    output.uv = input.uv;

    return output;
}