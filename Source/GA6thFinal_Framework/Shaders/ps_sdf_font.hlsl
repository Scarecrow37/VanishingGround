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

float4 ps_main(PSInput input) : SV_Target0
{
    float3 sampled = sdfTexture.Sample(samLinear_clamp, input.uv).rgb;
    float sd = Median(sampled.r, sampled.g, sampled.b);    
    float sigDist = (sd - 0.5) * sdfParams.PxRange;
    float screenPixelRange = fwidth(sigDist);
    
    float effectiveDist = sigDist + sdfParams.FontWeight;
    float fillOpacity = smoothstep(-screenPixelRange, screenPixelRange, effectiveDist);
    
    float4 color = bit32_4_fontColor.Color;
    
    if (sdfParams.Flags & ENABLE_OUTLINE)
    {
        float maxOutlineWidth = sdfParams.PxRange * 0.8;
        float clampedOutlineWidth = min(sdfParams.OutlineWidth, maxOutlineWidth);
        float outlineEdge = sigDist + sdfParams.FontWeight + clampedOutlineWidth;
        float outlineOpacity = smoothstep(-screenPixelRange, screenPixelRange, outlineEdge);
        float3 outlineColor = sdfParams.OutlineColor;
        color.rgb = lerp(outlineColor, color.rgb, fillOpacity / max(outlineOpacity, Epsilon));
        
        color.a *= outlineOpacity;
    }
    else
    {
        color.a *= fillOpacity;
    }
    
    clip(color.a - Epsilon);
    
    return color;
}