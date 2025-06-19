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
    
    float2 texel = 1 / float2(1920, 1080);
    
    float2 uv_up = input.uv - float2(0, texel.y);
    float2 uv_lt = input.uv - float2(texel.x, 0);
    float2 uv_rt = input.uv + float2(texel.x, 0);
    float2 uv_dn = input.uv + float2(0, texel.y);
    
    float4 color = screenTexture.Sample(samLinear_wrap, input.uv);
    //color += screenTexture.Sample(samLinear_wrap, uv_up);
    //color += screenTexture.Sample(samLinear_wrap, uv_lt);
    //color += screenTexture.Sample(samLinear_wrap, uv_rt);
    //color += screenTexture.Sample(samLinear_wrap, uv_dn);
    //color /= 5;
    
    float4 grayScale = 0;
    grayScale =  grayScaleTexture1024x1024.Sample(samLinear_wrap, input.uv);
    grayScale += grayScaleTexture512x512.Sample(samLinear_wrap, input.uv);
    grayScale += grayScaleTexture256x256.Sample(samLinear_wrap, input.uv);
    grayScale += grayScaleTexture128x128.Sample(samLinear_wrap, input.uv);   
    
    return (color + grayScale);// +color;
}