#include "PostProcess.hlsli"
#include "ShaderUtilities.hlsli"

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

#ifdef Lum
Texture2D txPostProcess : register(t2);
#else
Texture2D txLuminance : register(t2);
#endif
float4 main(PS_INPUT input) : SV_Target
{        
    //return txSource.Sample(samLinear_wrap, input.uv);
    
#ifdef Lum
    float3 color = txPostProcess.Sample(samLinear_wrap, input.uv).rgb;
    color = GammaToLinearSpace(color);
    float luminance = GetLuminance(color);
    return float4(luminance, luminance, luminance, 1);
#else
    float3 base = txSource.Sample(samLinear_wrap, input.uv).rgb;
    float3 texColor = GammaToLinearSpace(base);
    
    uint width, height, level;
    txLuminance.GetDimensions(0, width, height, level);
    float avgLuminance = txLuminance.SampleLevel(samLinear_wrap, input.uv, level - 1).r;
    float pixelLuminance = GetLuminance(texColor);
    float luminanceFactor = pixelLuminance / avgLuminance;   
    
    texColor *= 4;
    texColor *= (0.8 + 0.2 * luminanceFactor);

    float3 curr = Uncharted2Tonemap(texColor);

    float3 whiteScale = 1.0f / Uncharted2Tonemap(W);
    float3 color = curr * whiteScale;
      
    float3 retColor = LinearToGammaSpace(color);
    return float4(color, 1);
#endif
}