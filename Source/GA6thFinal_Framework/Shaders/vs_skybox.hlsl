#include "CommonData.hlsli"
struct VSInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float3 texCoord : TEXCOORD;
    float4 positionProj : SV_Position;
};

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    float4 pos = input.position;
    output.texCoord = pos.xyz;
    float3 pos3 = mul(pos.xyz, (float3x3) cameraData.View);
    pos = mul(float4(pos3, 1.f), cameraData.Projection);
    output.positionProj = pos;
    return output;
}
