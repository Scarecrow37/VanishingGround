#include "CommonData.hlsli"

StructuredBuffer<matrix> worldMatrices;
StructuredBuffer<uint> IDs;

struct VSInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    uint instanceID : SV_InstanceID;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation uint instanceID : TEXCOORD1;
};

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    
    float4 position = float4(input.position.xy, 1, 1);
    uint ID = IDs[input.instanceID];
        
    output.position = mul(position, worldMatrices[ID]);
    output.position = mul(output.position, cameraData.Projection);
    
    output.uv = input.uv;
    output.instanceID = input.instanceID;
    
    return output;
}