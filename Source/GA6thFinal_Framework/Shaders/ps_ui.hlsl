#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation uint instanceID : TEXCOORD1;
};

struct Material
{
    uint textureID;
    float Alpha;
};

struct UIMaterialData
{
    uint Type;
    float Fill;
};

StructuredBuffer<Material> material;
StructuredBuffer<UIMaterialData> uiMaterialData;
StructuredBuffer<uint> IDs : register(t0, space0);
Texture2D textures[];

// Material Types
static const uint BASIC = 0;
static const uint LINEAR_FILL = 1;

float4 ps_main(PSInput input) : SV_Target
{
    uint index = IDs[input.instanceID];
    
    float4 color = textures[material[index].textureID].Sample(samLinear_wrap, input.uv);
    color.a *= material[index].Alpha;
        
    switch (uiMaterialData[index].Type)
    {
        case LINEAR_FILL:
            clip(uiMaterialData[index].Fill - input.uv.x);
            break;
    }

    return color;
}