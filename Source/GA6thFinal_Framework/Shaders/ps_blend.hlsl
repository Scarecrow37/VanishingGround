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
Texture2D grayScaleTexture;

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
    color += screenTexture.Sample(samLinear_wrap, uv_up);
    color += screenTexture.Sample(samLinear_wrap, uv_lt);
    color += screenTexture.Sample(samLinear_wrap, uv_rt);
    color += screenTexture.Sample(samLinear_wrap, uv_dn);
    color /= 5;
    
    
    float4 gray = dot(color.rgb, float3(0.2126, 0.7152, 0.0722));
    float4 highlight = max(0, gray - 0.5f) * 2;
    
    texel = 1 / float2(480, 256);
    uv_up = input.uv - float2(0, texel.y);
    uv_lt = input.uv - float2(texel.x, 0);
    uv_rt = input.uv + float2(texel.x, 0);
    uv_dn = input.uv + float2(0, texel.y);
    
    float4 grayScale = grayScaleTexture.Sample(samLinear_wrap, input.uv);
    grayScale += grayScaleTexture.Sample(samLinear_wrap, uv_up);
    grayScale += grayScaleTexture.Sample(samLinear_wrap, uv_lt);
    grayScale += grayScaleTexture.Sample(samLinear_wrap, uv_rt);
    grayScale += grayScaleTexture.Sample(samLinear_wrap, uv_dn);
    
    grayScale /= 5;
    
    //smoothstep()
    return (color + color * highlight) + grayScale;
}