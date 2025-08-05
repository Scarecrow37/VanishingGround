#include "CommonData.hlsli"

static const float3 SSAOKernel[16] =
{
    float3(0.587, 0.245, 0.423), float3(-0.348, 0.378, 0.671),
    float3(0.234, -0.156, 0.854), float3(-0.479, 0.632, -0.124),
    float3(0.137, 0.892, -0.307), float3(-0.642, 0.187, 0.374),
    float3(0.214, -0.368, -0.784), float3(0.543, -0.732, 0.120),
    float3(-0.287, -0.476, 0.673), float3(0.726, -0.238, -0.304),
    float3(-0.123, 0.473, -0.789), float3(0.349, -0.832, 0.234),
    float3(-0.586, 0.125, 0.598), float3(0.142, 0.456, 0.759),
    float3(-0.437, -0.523, -0.312), float3(0.672, 0.374, 0.192)
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

struct SSAOProperty
{
    float radius;
    float falloff;
    float strengthFactor;
    float contrastFactor;
};

Texture2D normalMap;
Texture2D depthMap;
ConstantBuffer<SSAOProperty> bit32_4_ssaoProperty;

// radius 0.f ~ 1.f
// 리벨리온 radius = 0.0005f, fallOff = 3.0f, strengthFactor = 2.0f, contrastFactor = 2.0f

float ps_main(PSInput input) : SV_Target
{
    float3 normal = normalMap.SampleLevel(samLinear_wrap, input.uv, 0).xyz;
    float depth = depthMap.SampleLevel(samLinear_wrap, input.uv, 0).r;
    
    float occlusion = 1.f;
    // factor들 나중에 빼서 사용 가능
    
    SSAOProperty property = bit32_4_ssaoProperty;
    
    [unroll]
    for (int i = 0; i < 16; ++i)
    {
        float3 sampleDir = normalize(SSAOKernel[i]);
        float3 samplePos = float3(input.uv, depth) + sampleDir * property.radius;
        
        float3 sampleNormalData = normalMap.SampleLevel(samLinear_wrap, samplePos.xy, 0).xyz;
        float3 sampleNormal = normalize(sampleNormalData);
        float sampleDepth = depthMap.SampleLevel(samLinear_wrap, samplePos.xy, 0).r;
        
        float3 toSampleDir = normalize(samplePos - float3(input.uv, depth));
        float angle = max(dot(normal, sampleNormal), 0.f);
        float depthDiff = samplePos.z - sampleDepth;
        if (depthDiff > 0.f && depthDiff < property.radius)
        {
            float weight = exp(-depthDiff * property.falloff);
            occlusion += saturate(weight * angle);
        };
    }
    occlusion = saturate(occlusion / 16.f) * property.strengthFactor;
    occlusion = pow(saturate(occlusion), property.contrastFactor);
    
    return occlusion;
};
