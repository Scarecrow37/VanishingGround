#include "CommonData.hlsli"


struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation uint instanceID : TEXCOORD1;
};

struct Material
{
    uint ID;
    float Alpha;
};

StructuredBuffer<Material> material;
StructuredBuffer<uint> IDs;
Texture2D textures[];

float4 ps_main(PSInput input) : SV_Target
{
    uint ID = IDs[input.instanceID];
    
    float4 color = textures[material[ID].ID].Sample(samLinear_wrap, input.uv);
    
    color.a *= material[ID].Alpha;;
    color.rgb *= color.a;   
    
    return color;
}