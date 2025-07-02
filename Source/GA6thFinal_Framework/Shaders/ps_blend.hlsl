#include "CommonData.hlsli"
#include "Function.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

static const float A = 0.15;
static const float B = 0.50;
static const float C = 0.10;
static const float D = 0.20;
static const float E = 0.02;
static const float F = 0.30;
static const float W = 11.2;

float3 Uncharted2Tonemap(float3 x)
{
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float GetLuminance(float3 color)
{
    return dot(color, float3(0.2126, 0.7152, 0.0722));
}

Texture2D screenTexture;
Texture2D sourceTexture;
//Texture2D uiTexture;

float4 ps_main(PS_INPUT input) : SV_TARGET
{
    float4 color = screenTexture.Sample(samLinear_wrap, input.uv);
    float4 sourceColor = sourceTexture.Sample(samLinear_wrap, input.uv);
    float4 finalColor = color + sourceColor;
    
    float tempHardExpoure = 1.0f;
    float exposureBias = 1.0f;
    
    finalColor *= tempHardExpoure * exposureBias;

    float3 curr = Uncharted2Tonemap(finalColor.rgb);
    float3 whiteScale = 1.0f / Uncharted2Tonemap(W);
    float3 result = LinearToGammaSpace(curr * whiteScale);
    
    return float4(result, 1.0f);
}
