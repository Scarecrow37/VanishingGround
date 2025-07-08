#include "CommonData.hlsli"
#include "Function.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D<float4> screenTexture;
Texture2D<uint> customDepthTexture;

float4 ps_main(PS_INPUT input) : SV_TARGET
{    
    float4 color = screenTexture.Sample(samLinear_wrap, input.uv);
        
    float luminance = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    float threshold = 1.0f;
    
    float4 result = color * CalculatePostProcessMask(customDepthTexture, input.uv);

    return result;
}