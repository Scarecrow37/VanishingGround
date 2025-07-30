#include "CommonData.hlsli"
#include "Function.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct BloomProperty
{
    float Threshold;
    float Intensity;
    float BloomKnee;
};

Texture2D<float4> screenTexture;
Texture2D<uint> customDepthTexture;

ConstantBuffer<BloomProperty> bit32_3_bloomProperty;

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    float4 color = screenTexture.Sample(samLinear_wrap, input.uv);

    float luminance = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    
    BloomProperty property = bit32_3_bloomProperty;
    float softThreshold = property.Threshold - property.BloomKnee;

    float contribution = saturate((luminance - softThreshold) / property.BloomKnee * 2.0);
    float4 result = contribution * color * property.Intensity;

    return result * CalculatePostProcessMask(customDepthTexture, input.uv);
}