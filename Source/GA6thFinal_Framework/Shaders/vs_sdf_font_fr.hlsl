#include "CommonData.hlsli"
#include "SDFDatas.hlsli"

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

StructuredBuffer<matrix> ui_matrices;

VSOutput vs_main(VSInput input)
{    
    VSOutput output = (VSOutput) 0;       
    
    output.position = mul(input.position, ui_matrices[sdfParams.InstanceID]);
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);
    
    output.uv = input.uv;
    
    return output;
}