#include "CommonData.hlsli"

struct VSInput
{
    float4 position     : POSITION;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 biTangent    : BINORMAL;
    float2 uv           : TEXCOORD;
    float2 lightUV      : TEXCOORD1;
    
    uint instanceID : SV_InstanceID;
};

struct VSOutput
{
    float4 position      : SV_POSITION;
    float3 normal        : NORMAL;
    float3 tangent       : TANGENT;
    float3 biTangent     : BINORMAL;
    float2 uv            : TEXCOORD;
    float4 worldPosition : TEXCOORD1;
    
    nointerpolation uint4 materialID  : TEXCOORD2;
    nointerpolation uint  customDepth : TEXCOORD3;
    nointerpolation float alpha       : TEXCOORD4;
};

struct Offset
{
    uint Offset;
};

ConstantBuffer<Offset> bit32_1_offset;

VSOutput vs_main(VSInput input)
{
    uint offset = bit32_1_offset.Offset;
    InstanceData data = instanceData[input.instanceID + offset];
    
    VSOutput output = (VSOutput) 0;
    
    output.position = mul(input.position, matrices[data.MatrixID].World);
    output.worldPosition = output.position;
    output.position = mul(output.position, cameraData.View);
    output.position = mul(output.position, cameraData.Projection);

    output.normal = normalize(mul(input.normal, (float3x3) matrices[data.MatrixID].InverseTranspose));
    output.tangent = normalize(mul(input.tangent, (float3x3) matrices[data.MatrixID].InverseTranspose));
    output.biTangent = normalize(mul(input.biTangent, (float3x3) matrices[data.MatrixID].InverseTranspose));

    output.uv = input.uv;
    output.materialID = data.MaterialID;
    output.customDepth = data.CustomDepth;
    output.alpha = data.Alpha;

    return output;
}