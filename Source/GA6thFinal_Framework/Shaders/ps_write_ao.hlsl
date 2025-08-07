#include "CommonData.hlsli"
#include "Function.hlsli"
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
    float threshold;
};

Texture2D normalMap;
Texture2D depthMap;
ConstantBuffer<SSAOProperty> bit32_5_ssaoProperty;



float ps_main(PSInput input) : SV_TARGET
{
    float depth = depthMap.SampleLevel(samLinear_wrap, input.uv, 0).r;
    SSAOProperty property = bit32_5_ssaoProperty;
    clip(depth - property.threshold);
    
    float3 normal = normalize(normalMap.SampleLevel(samLinear_wrap, input.uv, 0).xyz);

    float3 viewPos = ReconstructViewPos(input.uv, depth);

    // Tangent basis from normal
    float3 up = abs(normal.z) < 0.999 ? float3(0, 0, 1) : float3(0, 1, 0);
    float3 tangent = normalize(cross(up, normal));
    float3 bitangent = cross(normal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normal);

    float occlusion = 0.0f;
    float radius = property.radius/100.f;
    [unroll]
    for (int i = 0; i < 16; ++i)
    {
        float3 sampleOffset = mul(TBN, SSAOKernel[i]) * radius;
        float3 samplePosVS = viewPos + sampleOffset;

        float2 sampleUV = ProjectToUV(samplePosVS);
        if (sampleUV.x < 0.0 || sampleUV.x > 1.0 || sampleUV.y < 0.0 || sampleUV.y > 1.0)
            continue;

        float sampleDepth = depthMap.SampleLevel(samLinear_wrap, sampleUV, 0).r;
        float3 sampleNormal = normalize(normalMap.SampleLevel(samLinear_wrap, sampleUV, 0).xyz);
        float3 sampleViewPos = ReconstructViewPos(sampleUV, sampleDepth);

        float angle = max(dot(normal, sampleNormal), 0.0f);
        float depthDiff = samplePosVS.z - sampleViewPos.z;

        if (depthDiff > 0.0f && depthDiff < radius)
        {
            float weight = exp(-depthDiff * property.falloff);
            occlusion += saturate(weight * angle);
        }
    }

    occlusion = saturate(occlusion / 16.0f) * property.strengthFactor;
    occlusion = pow(saturate(occlusion), property.contrastFactor);
    return occlusion;
}

