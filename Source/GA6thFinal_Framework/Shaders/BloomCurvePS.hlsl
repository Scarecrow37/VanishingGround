#include "PostProcess.hlsli"
#include "ShaderUtilities.hlsli"

cbuffer BloomCurveDesc : register(b1)
{
    float weight;
    float threshold;
}

float GetBloomCurve(float intensity, float threshold)
{
    float result = intensity;
    intensity *= 2;
    result = max(0, intensity - threshold);
    result *= result;
    return result * 0.5;
}

float4 main(PS_INPUT input) : SV_Target
{
    float3 source = GammaToLinearSpace(txSource.Sample(samLinear_wrap, input.uv).rgb);
    float3 factor = 0.3;
    float intensity = dot(source, factor);
    float bloom_intensity = GetBloomCurve(intensity, 1);
    float3 bloom_color = source * bloom_intensity / intensity;
    
    return float4(LinearToGammaSpace(bloom_color), 1) * LayerMasking(input.uv);
}