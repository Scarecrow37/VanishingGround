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
    float alpha;
    uint4 atlas;
};

struct UIMaterialData
{
    uint type;
    float fill;
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
    uint index = input.instanceID;
    
    float2 column_row = (float2) material[index].atlas.xy;
    float2 current = (float2) material[index].atlas.zw;

    float2 offset = 1 / column_row;
    float2 uv = input.uv / column_row;

    float4 color = textures[material[index].ID].Sample(samLinear_wrap, offset * current + uv);
    color.a *= material[index].alpha;
    
    clip(color.a - Epsilon);
    
    switch (uiMaterialData[index].type)
    {
        case LINEAR_FILL:
            clip(uiMaterialData[index].fill - input.uv.x);
            break;
    }

    return color;
}