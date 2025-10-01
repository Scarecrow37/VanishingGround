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

struct SDFParams
{
    uint InstanceID;
    float PxRange;
    float FontWeight;
};

Texture2D sdfTexture;
ConstantBuffer<FontColor> bit32_4_fontColor;
ConstantBuffer<SDFParams> bit32_3_sdfParams;

RWTexture2D<uint> OITHead;
RWStructuredBuffer<OITNode> OITNodes;
RWByteAddressBuffer OITCounter;

void ps_main(PSInput input)
{
    float3 sampled = sdfTexture.Sample(samLinear_clamp, input.uv).rgb;
    float sd = Median(sampled.r, sampled.g, sampled.b);    
    float sigDist = (sd - 0.5) * bit32_3_sdfParams.PxRange;
    float screenPixelRange = fwidth(sigDist);
    float effectiveDist = sigDist + bit32_3_sdfParams.FontWeight;
    float opacity = smoothstep(-screenPixelRange, screenPixelRange, effectiveDist);
   
    float4 color = bit32_4_fontColor.Color;
    color.a *= opacity;
    clip(color.a - Epsilon);
    
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
