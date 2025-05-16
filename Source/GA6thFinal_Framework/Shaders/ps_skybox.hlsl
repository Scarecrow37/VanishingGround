#include "CommonData.hlsli"
TextureCube evnTexture;
struct VSOutput
{
    float3 texCoord : TEXCOORD;
    float4 positionProj : SV_Position;
};

float4 ps_main(VSOutput input) : SV_Target
{
    float4 color = evnTexture.Sample(samLinear_wrap, input.texCoord);
    color = pow(color, 1 / 2.2);
    return color;
}
