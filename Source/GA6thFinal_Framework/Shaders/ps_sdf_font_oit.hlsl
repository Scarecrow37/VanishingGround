#include "Function.hlsli"
#include "SDFDatas.hlsli"

float Median(float r, float g, float b)
{
    return max(min(r, g), min(max(r, g), b));
}

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D sdfTexture;

RWTexture2D<uint> OITHead;
RWStructuredBuffer<OITNode> OITNodes;
RWByteAddressBuffer OITCounter;

void ps_main(PSInput input)
{
    float3 sampled = sdfTexture.Sample(samLinear_clamp, input.uv).rgb;
    float sd = Median(sampled.r, sampled.g, sampled.b);
    float sigDist = (sd - 0.5) * sdfParams.PxRange;
    float screenPixelRange = fwidth(sigDist);
    
    float effectiveDist = sigDist + sdfParams.FontWeight;
    float fillOpacity = smoothstep(-screenPixelRange, screenPixelRange, effectiveDist);
    
    float4 color = bit32_4_fontColor.Color;
    color.a *= fillOpacity;
    color = Premultiply(color);
    
    if (sdfParams.Flags & ENABLE_OUTLINE)
    {
        float outlineEdge = sigDist + sdfParams.FontWeight + sdfParams.PxRange;
        float outlineOpacity = smoothstep(-screenPixelRange, screenPixelRange, outlineEdge);
                
        float4 outlineColor = sdfParams.OutlineColor;
        outlineColor.a *= outlineOpacity;
        outlineColor.a = outlineColor.a * (1.0 - fillOpacity);

        color += Premultiply(outlineColor);
    }
    
    clip(color.a - Epsilon);
    
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
