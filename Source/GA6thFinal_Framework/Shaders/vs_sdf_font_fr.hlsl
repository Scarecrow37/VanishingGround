#include "CommonData.hlsli"

StructuredBuffer<matrix> ui_matrices;

struct VSInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation uint instanceID : TEXCOORD1;
};
 
struct InstanceID
{
    uint ID;
};

ConstantBuffer<InstanceID> bit32_1_instanceID;

VSOutput vs_main(VSInput input)
{
    uint ID = bit32_1_instanceID.ID;
    
    VSOutput output = (VSOutput) 0;
    
    float4 position = float4(input.position.xyz, 1);
        
    output.position = mul(position, ui_matrices[ID]);
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);
    
    output.uv = input.uv;
    output.instanceID = ID;
    
    return output;
}