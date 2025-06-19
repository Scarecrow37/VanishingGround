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

Texture2D screenTexture;
Texture2D grayScaleTexture1024x1024;
Texture2D grayScaleTexture512x512;
Texture2D grayScaleTexture256x256;
Texture2D grayScaleTexture128x128;

float4 BoxFilter(float2 uv, float2 texelSize, Texture2D tex, SamplerState sam)
{
    float2 uv_up = uv - float2(0, texelSize.y);
    float2 uv_lt = uv - float2(texelSize.x, 0);
    float2 uv_rt = uv + float2(texelSize.x, 0);
    float2 uv_dn = uv + float2(0, texelSize.y);
    float4 color = tex.Sample(samLinear_wrap, uv);
    color += tex.Sample(sam, uv_up);
    color += tex.Sample(sam, uv_lt);
    color += tex.Sample(sam, uv_rt);
    color += tex.Sample(sam, uv_dn);
    
    return color / 5.0f;
}

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    //float4 color = screenTexture.Sample(samLinear_wrap, input.uv);
    
    //float2 texel = 1 / float2(120, 64);
    //float4 grayScale = grayScaleTexture.Sample(samLinear_wrap, input.uv);   
    
    //float2 uv_up = input.uv - float2(0, texel.y);
    //float2 uv_lt = input.uv - float2(texel.x, 0);
    //float2 uv_rt = input.uv + float2(texel.x, 0);
    //float2 uv_dn = input.uv + float2(0, texel.y);
    
    //grayScale += grayScaleTexture.Sample(samLinear_wrap, uv_up);
    //grayScale += grayScaleTexture.Sample(samLinear_wrap, uv_lt);
    //grayScale += grayScaleTexture.Sample(samLinear_wrap, uv_rt);
    //grayScale += grayScaleTexture.Sample(samLinear_wrap, uv_dn);
    
    //grayScale /= 5;
    
    ////smoothstep()
    //return color + grayScale;
    
    float4 color = BoxFilter(input.uv, postProcessData.TexelSize, screenTexture, samLinear_wrap);
    float4 grayScale = BoxFilter(input.uv, 1 / float2(1024, 1024), grayScaleTexture1024x1024, samLinear_clamp);
    grayScale += BoxFilter(input.uv, 1 / float2(512, 512), grayScaleTexture512x512, samLinear_clamp);
    grayScale += BoxFilter(input.uv, 1 / float2(256, 256), grayScaleTexture256x256, samLinear_clamp);    
    grayScale += BoxFilter(input.uv, 1 / float2(128, 128), grayScaleTexture128x128, samLinear_clamp);
    
    return color + grayScale;// +color;
}