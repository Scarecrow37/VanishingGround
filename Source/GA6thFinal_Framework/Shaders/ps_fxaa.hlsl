#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
};

Texture2D screenColor;
ConstantBuffer<FXAAData> bit32_5_fxaaProperty;

float CalculateLuminance(float3 rgb)
{
    return dot(rgb, float3(0.299, 0.587, 0.114));
}

float4 ps_main(PSInput input) : SV_Target
{
    float2 uv = input.uv;
    float3 centerColor = screenColor.SampleLevel(samLinear_clamp, uv, 0).rgb;
    float centerLuminance = CalculateLuminance(centerColor);

    // offset
    float2 offset = bit32_5_fxaaProperty.InverseResolution;
    float3 northColor = screenColor.SampleLevel(samLinear_clamp, uv + float2(0, -offset.y), 0).rgb;
    float3 southColor = screenColor.SampleLevel(samLinear_clamp, uv + float2(0, offset.y), 0).rgb;
    float3 westColor = screenColor.SampleLevel(samLinear_clamp, uv + float2(-offset.x, 0), 0).rgb;
    float3 eastColor = screenColor.SampleLevel(samLinear_clamp, uv + float2(offset.x, 0), 0).rgb;

    float lumN = CalculateLuminance(northColor);
    float lumS = CalculateLuminance(southColor);
    float lumW = CalculateLuminance(westColor);
    float lumE = CalculateLuminance(eastColor);

    float minLum = min(centerLuminance, min(min(lumN, lumS), min(lumW, lumE)));
    float maxLum = max(centerLuminance, max(max(lumN, lumS), max(lumW, lumE)));
    float lumRange = maxLum - minLum;

    float threshold = max(bit32_5_fxaaProperty.QualityEdgeDetectionThreshold * maxLum,
                          bit32_5_fxaaProperty.QualityMinimumEdgeThreshold);

    // 엣지가 아니면 블러
    if (lumRange < threshold)
    {
        float subPixel = saturate((lumRange - threshold * 0.25f) / (threshold + 1e-5f));
        subPixel = pow(subPixel, 0.5f);
        return float4(lerp(centerColor, (northColor + southColor + eastColor + westColor) * 0.25f, subPixel * bit32_5_fxaaProperty.QualitySubpixel), 1);
    }
    
    float2 edgeDir;
    edgeDir.x = -((lumN + lumS) - 2 * centerLuminance);
    edgeDir.y = ((lumW + lumE) - 2 * centerLuminance);
    edgeDir /= (abs(edgeDir.x) + abs(edgeDir.y) + Epsilon);
    
    float2 step = edgeDir * bit32_5_fxaaProperty.InverseResolution;
    float3 sample1 = screenColor.SampleLevel(samLinear_clamp, uv + step * 0.5f, 0).rgb;
    float3 sample2 = screenColor.SampleLevel(samLinear_clamp, uv - step * 0.5f, 0).rgb;

    float3 blendColor = (sample1 + sample2) * 0.5f;
    float3 finalColor = lerp(centerColor, blendColor, bit32_5_fxaaProperty.QualitySubpixel);

    return float4(finalColor, 1.0f);
}
