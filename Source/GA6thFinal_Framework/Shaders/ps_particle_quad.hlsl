#include "CommonData.hlsli"

Texture2D AlbedoTextures[];

//test
//Texture2D AlbedoTextures;

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    float4 color : COLOR0;
    uint emitterIndex : TEXCOORD1;

};

float4 ps_main(PSInput input) : SV_Target
{
    float factor = AlbedoTextures[input.emitterIndex].Sample(samPoint_clamp, input.uv);
    float4 output = float4(input.color.rgb, input.color.a * factor );
    //float4 output = float4(input.color.rgb, max(1, factor));
    return output;

}