#include "CommonData.hlsli"

StructuredBuffer<matrix> ui_matrices;
StructuredBuffer<uint> IDs : register(t0, space0);

struct VSInput
{
    float4 position : POSITION;
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
    
    float4 position = float4(input.position.xyz, 1);
    uint ID = IDs[input.instanceID];
        
    output.position = mul(position, ui_matrices[ID]);
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);
    
    output.uv = input.uv;
    output.instanceID = input.instanceID;
    
    return output;
}