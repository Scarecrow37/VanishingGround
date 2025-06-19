#include "CommonData.hlsli"
#include "Function.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 grayScale1024x1024 : SV_TARGET0;
    float4 grayScale512x512   : SV_TARGET1;
    float4 grayScale256x256   : SV_TARGET2;
    float4 grayScale128x128   : SV_TARGET3;
};

Texture2D<float4> screenTexture;
Texture2D<uint> customDepthTexture;

PS_OUTPUT ps_main(PS_INPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0;
    
    float4 color = screenTexture.Sample(samLinear_wrap, input.uv);
        
    float luminance = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    float threshold = 0.5f;
    
    float4 result = max(0, luminance - threshold) * 2 * CalculatePostProcessMask(customDepthTexture, input.uv);
    
    output.grayScale1024x1024 = result;
    output.grayScale512x512   = result;
    output.grayScale256x256   = result;
    output.grayScale128x128   = result;

    return output;
}
