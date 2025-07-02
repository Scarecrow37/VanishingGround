#include "CommonData.hlsli"
#include "Function.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

struct NumTextures
{
    uint numTextures;
};

ConstantBuffer<NumTextures> bit32_1_numTextures;
Texture2D textures[];

float4 ps_main(PS_INPUT input) : SV_TARGET
{    
    float4 grayScale = 0;
    
    for (uint i = 0; i < bit32_1_numTextures.numTextures; i++)
    {
        grayScale += textures[i].Sample(samLinear_clamp, input.uv);
    };
    
    //grayScale += grayScaleTexture0.Sample(samLinear_clamp, input.uv);
    //grayScale += grayScaleTexture1.Sample(samLinear_clamp, input.uv);
    //grayScale += grayScaleTexture2.Sample(samLinear_clamp, input.uv);
    //grayScale += grayScaleTexture3.Sample(samLinear_clamp, input.uv);
    //grayScale += grayScaleTexture4.Sample(samLinear_clamp, input.uv);
    
    return grayScale;
}