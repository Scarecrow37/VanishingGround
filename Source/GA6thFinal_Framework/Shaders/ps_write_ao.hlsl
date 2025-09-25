#include "CommonData.hlsli"
#include "Function.hlsli"


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
    
    if (depth < property.threshold)
        return 1.0f;

    float3 normal = normalize(normalMap.SampleLevel(samLinear_wrap, input.uv, 0).xyz);
    float3 viewPos = ReconstructViewPos(input.uv, depth);

    float3 up = abs(normal.z) < 0.999 ? float3(0, 0, 1) : float3(0, 1, 0);
    float3 tangent = normalize(cross(up, normal));
    float3 bitangent = cross(normal, tangent);
    float3x3 TBN = float3x3(tangent, bitangent, normal);

    float occlusion = 0.0f;
    float radius = property.radius / 100.f;

    [unroll]
    for (int i = 0; i < 16; ++i)
    {
        float3 sampleOffset = mul(RandomKernel[i], TBN) * radius;
        float3 samplePosVS = viewPos + sampleOffset;

        float2 sampleUV = ProjectToUV(samplePosVS);
        if (sampleUV.x < 0.0 || sampleUV.x > 1.0 || sampleUV.y < 0.0 || sampleUV.y > 1.0)
            continue;

        float sampleDepth = depthMap.SampleLevel(samLinear_wrap, sampleUV, 0).r;
        float3 sampleNormal = normalize(normalMap.SampleLevel(samLinear_wrap, sampleUV, 0).xyz);
        float3 sampleViewPos = ReconstructViewPos(sampleUV, sampleDepth);

        float3 offsetVec = sampleViewPos - viewPos;
        float dist = length(offsetVec);
        float angle = max(dot(normal, sampleNormal), 0.0f);

        if (dist < radius)
        {
            float weight = exp(-dist * property.falloff);
            float rangeCheck = smoothstep(0.0f, 1.0f, radius / (dist + 1e-5)); // soft range filter
            occlusion += saturate(weight * angle) * rangeCheck;
        }
    }

    occlusion = saturate(occlusion / 16.0f) * property.strengthFactor;
    occlusion = pow(saturate(occlusion), property.contrastFactor);
    return occlusion;
}

