#include "Function.hlsli"

float Median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct FontColor
{
    float4 Color;
};

Texture2D sdfTexture;
ConstantBuffer<FontColor> bit32_4_fontColor;

RWTexture2D<uint> OITHead;
RWStructuredBuffer<OITNode> OITNodes;
RWByteAddressBuffer OITCounter;

void ps_main(PSInput input)
{
    float3 sample = sdfTexture.Sample(samLinear_clamp, input.uv).rgb;
    float sigDist = Median(sample.r, sample.g, sample.b);
    float screenPxDistance = sigDist - 0.5;
    float opacity = clamp(screenPxDistance / fwidth(screenPxDistance) + 0.5, 0.0, 1.0);
   
    clip(opacity - 0.1);

    float4 color = Premultiply(bit32_4_fontColor.Color);
    
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
