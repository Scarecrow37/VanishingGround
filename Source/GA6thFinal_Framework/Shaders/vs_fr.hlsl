#include "CommonData.hlsli"

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



VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    float3 normal = normalize(input.normal);
    float3 tangent = normalize(cross(float3(0, 1, 0), normal));
    float3 biTangent = normalize(cross(normal, tangent));
    
    output.position = mul(input.position, objectData[object.ID].World);
    
    output.worldPosition = output.position;
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);

    //output.normal = normalize(mul(input.normal, (float3x3) objectData[object.ID].World));
    //output.tangent = normalize(mul(input.tangent, (float3x3) objectData[object.ID].World));
    //output.biTangent = normalize(mul(input.biTangent, (float3x3) objectData[object.ID].World));
    
    output.normal = normalize(mul(normal, (float3x3) objectData[object.ID].World));
    output.tangent = normalize(mul(tangent, (float3x3) objectData[object.ID].World));
    output.biTangent = normalize(mul(biTangent, (float3x3) objectData[object.ID].World));
    
    output.uv = input.uv;

    return output;
}