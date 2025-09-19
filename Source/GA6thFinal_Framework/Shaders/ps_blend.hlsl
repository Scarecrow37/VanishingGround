#include "CommonData.hlsli"
#include "Function.hlsli"

struct PSInput
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

struct TonMappingProperty
{
    float3 WhiteBalance;
    float Exposure;
    float Saturation;
    float Contrast;
};

Texture2D screenTexture;
Texture2D sourceTexture;

ConstantBuffer<TonMappingProperty> bit32_6_tonMappingProperty;

float4 ps_main(PSInput input) : SV_TARGET
{
    float3 hdrColor = screenTexture.Sample(samLinear_wrap, input.uv).rgb;
    float3 postProcessColor = sourceTexture.Sample(samLinear_wrap, input.uv).rgb;

    hdrColor += postProcessColor;
    
    TonMappingProperty property = bit32_6_tonMappingProperty;
    
    float3 exposedColor = hdrColor * property.Exposure;
    
    float3 tonemappedColor = Uncharted2Tonemap(exposedColor);
    
    float3 whiteScale = 1.0 / Uncharted2Tonemap(W);
    tonemappedColor *= whiteScale;

    tonemappedColor *= property.WhiteBalance;

    // Contrast
    tonemappedColor = 0.5f + property.Contrast * (tonemappedColor - 0.5f);

    // Saturation
    float luminance = GetLuminance(tonemappedColor);
    tonemappedColor = lerp(luminance.xxx, tonemappedColor, property.Saturation);
    
    float3 result = LinearToGammaSpace(tonemappedColor * whiteScale);

    return float4(result, 1.0f);
}
