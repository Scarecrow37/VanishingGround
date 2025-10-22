#include "CommonData.hlsli"
#include "Function.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};
struct SSRProperty
{
    float maxThickness; // ray depth test 허용 오차
    float stepSize; // ray 길이 간격
    float maxRayCount; // 최대 ray step 갯수
    float screenFade;
};


Texture2D screenColor;
Texture2D screenNormal;
Texture2D screenDepth;
Texture2D screenORM;
ConstantBuffer<SSRProperty> bit32_4_ssrProperty;

float noise(float2 seed)
{
    return frac(sin(dot(seed.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float4 ps_main(PSInput input) : SV_Target
{
    float depth = screenDepth.SampleLevel(samLinear_clamp, input.uv, 0).r;
    float3 normal_world = screenNormal.SampleLevel(samLinear_clamp, input.uv, 0).xyz;
    float4 ormData = screenORM.SampleLevel(samLinear_clamp, input.uv, 0);
    float roughness = ormData.g;
    float metallic = ormData.b;
    
    float3 baseColor = screenColor.SampleLevel(samLinear_clamp, input.uv, 0).rgb;

    float3 normal_view = normalize(mul(normal_world, (float3x3) cameraData.View));
    float3 viewPos = ReconstructViewPos(input.uv, depth);
    float3 viewDir = normalize(viewPos);
    float3 reflectDir = reflect(viewDir, normal_view);
    bool foundHit = false;
    float2 hitUV = float2(0.0f, 0.0f);
    SSRProperty property = bit32_4_ssrProperty;
    float3 currentRayPos = viewPos;
    [loop]
    for (int i = 0; i < property.maxRayCount; ++i)
    {
        currentRayPos += reflectDir * property.stepSize;
        float2 sampleUV = ProjectToUV(currentRayPos);
        if (sampleUV.x < 0.0f || sampleUV.x > 1.0f || sampleUV.y < 0.0f || sampleUV.y > 1.0f)
        {
            break;
        }
        float sceneDepthAtSample = screenDepth.SampleLevel(samLinear_clamp, sampleUV, 0).r;
        float3 sceneViewPosAtSample = ReconstructViewPos(sampleUV, sceneDepthAtSample);
        
        float depthDiff = sceneViewPosAtSample.z - currentRayPos.z;
        
        if (depthDiff > 0 && depthDiff < property.maxThickness)
        {
            foundHit = true;
            hitUV = sampleUV;
            break;
        }
    }
    
    float3 finalColor = baseColor;
    if (foundHit)
    {
        float3 reflectionColor = screenColor.SampleLevel(samLinear_clamp, hitUV, 0).rgb;
        float reflectionStrength = saturate(1.0f - roughness); // roughness=0 → full reflection, roughness=1 → no reflection
        float2 distToCenter = abs(hitUV - 0.5) * 2.f;
        float fade = pow(saturate(1.f - distToCenter.x), property.screenFade * 0.5f) * pow(saturate(1.f - distToCenter.y * 0.5f), property.screenFade);

        finalColor = lerp(finalColor, reflectionColor, fade * reflectionStrength);
    }
    
    return float4(finalColor, 1.0f);

}