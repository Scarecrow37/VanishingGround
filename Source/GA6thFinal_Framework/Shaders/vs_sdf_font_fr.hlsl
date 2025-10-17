#include "CommonData.hlsli"

struct VSInput
{
    float4 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct SDFParams
{
    uint InstanceID;
    float PxRange;
    float FontWeight;
};

StructuredBuffer<matrix> text_matrices;
ConstantBuffer<SDFParams> bit32_3_sdfParams;

VSOutput vs_main(VSInput input)
{    
    VSOutput output = (VSOutput) 0;       
    
    output.position = mul(input.position, text_matrices[bit32_3_sdfParams.InstanceID]);
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);
    
    output.uv = input.uv;
    
    return output;
}