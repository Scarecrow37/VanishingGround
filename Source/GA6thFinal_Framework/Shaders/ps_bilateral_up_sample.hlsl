#include "Function.hlsli"

Texture2D<float4> ssgiHalf;
Texture2D<float4> screenNormal;
Texture2D<float> screenDepth;

ConstantBuffer<SSGIData> ssgiData;

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

float Gaussian(float x, float sigma)
{
    return exp(-(x * x) / (2.0 * sigma * sigma));
}

float4 ps_main(PSInput input): SV_Target
{
    float2 uvFull = input.uv;
    float2 uvHalf = uvFull;

    float3 centerNormal = normalize(screenNormal.SampleLevel(samLinear_clamp, uvFull, 0).xyz);
    float centerDepth = screenDepth.SampleLevel(samLinear_clamp, uvFull,0).r;

    float3 sum = float3(0, 0, 0);
    float wsum = 0.0;
    float halfW = ssgiData.ScreenSize.x;
    float halfH = ssgiData.ScreenSize.y;
    // 3x3 kernel in half-res
    [unroll]
    for (int y = -1; y <= 1; ++y)
    {
        [unroll]
        for (int x = -1; x <= 1; ++x)
        {
            float2 offset = float2(x, y) / float2(halfW, halfH);
            float2 sampleUV = uvHalf + offset;
            if (sampleUV.x < 0 || sampleUV.x > 1 || sampleUV.y < 0 || sampleUV.y > 1)
                continue;

            float4 s = ssgiHalf.SampleLevel(samLinear_clamp, sampleUV, 0);
            float sampleDepth = screenDepth.SampleLevel(samLinear_clamp, sampleUV, 0).r;
            float3 sampleNormal = normalize(screenNormal.SampleLevel(samLinear_clamp, sampleUV, 0).xyz);

            float depthWeight = Gaussian(abs(sampleDepth - centerDepth), ssgiData.DepthSigma);
            float normalWeight = Gaussian(1.0 - dot(sampleNormal, centerNormal), ssgiData.NormalSigma);
            float w = depthWeight * normalWeight;

            sum += s.rgb * w;
            wsum += w;
        }
    }
        
    float3 color = (wsum > 0.0) ? sum / wsum : float3(0, 0, 0);
    return float4(color,1.0);
}