#include "Function.hlsli"

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

RWTexture2D<uint> OITHead;
RWStructuredBuffer<OITNode> OITNodes;
RWByteAddressBuffer OITCounter;

Texture2D textures[];

// Material Types
static const uint BASIC = 0;
static const uint LINEAR_FILL = 1;
static const uint RADIAL_FILL = 2;

void ps_main(PSInput input)
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
        case RADIAL_FILL:
            float2 centered = input.uv - 0.5;
            float angle = atan2(centered.x, centered.y);
            float normalizedAngle = (angle + PI) / (2.0 * PI);
            clip(uiMaterialData[index].fill - normalizedAngle);
            break;
        default:
            break;
    }
    
    color = Premultiply(color);

    uint nodeIndex = OITAllocNode(OITCounter);
    if (nodeIndex >= FRAME_NODE_CAPACITY)
    {
        return;
    }
    
    uint2 pix = uint2(input.position.xy);
    
    uint oldHead;
    InterlockedExchange(OITHead[pix], nodeIndex, oldHead);
    
    OITNode node;
    node.Color = color;
    node.Depth = input.position.z;
    node.Next = (oldHead == 0xFFFFFFFF) ? OIT_NULL : oldHead;    
    OITNodes[nodeIndex] = node;   
}