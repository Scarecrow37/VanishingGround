#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

struct SSRProperty
{
    float maxDistance; // ssr최대 반사거리
    float stride; // 레이 마칭 단위 거리
    float thickness; // 충돌 오차
};

Texture2D screenColor;
Texture2D screenNormal;
Texture2D screenDepth;
Texture2D screenORM;
ConstantBuffer<SSRProperty> bit32_3_ssrProperty;

float3 ReconstructViewPos(float2 uv, float depth)
{
    float2 ndc = uv * 2.0f - 1.0f; // Convert to NDC
    float4 clipSpacePos = float4(ndc, depth, 1.0f);
    float4 viewSpacePos = mul(clipSpacePos, cameraData.ProjectionInverse);
    viewSpacePos.xyz /= viewSpacePos.w; // Perspective divide
    return viewSpacePos.xyz;
}

float3 RayMarchSSR(float3 viewPos, float3 reflectionDir, out float2 hitUV,float depth)
{
    float3 position = viewPos;
    SSRProperty property = bit32_3_ssrProperty;
    
    for (int i = 0; i < 64;++i)
    {
        position += reflectionDir * property.stride;
        
        float4 projectionPosition = mul(float4(position, 1.f), cameraData.Projection);
        projectionPosition.xyz /= projectionPosition.w; // Perspective divide
        
        float2 uv = projectionPosition.xy * 0.5f + 0.5f; // Convert to UV
        if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f) 
            break;
        
        float3 screenViewPosition = ReconstructViewPos(uv, depth);
        float distance = length(position - screenViewPosition);
        
        if (distance < property.thickness)
        {
            hitUV = uv;
            return position;
        }
        if (length(position - viewPos) > property.maxDistance)
            break;
    }
    hitUV = float2(-1.f, -1.f); // No hit
    return float3(0.f, 0.f, 0.f); // Return zero vector if no hit
}

float4 ps_main(PSInput input) : SV_Target
{
    float2 uv = input.uv;
    // alreay normalized in gbuffer pass
    float3 normal = screenNormal.SampleLevel(samLinear_clamp, uv, 0).xyz;
    float depth = screenDepth.SampleLevel(samLinear_clamp, uv, 0).r;
    
    float3 viewPos = ReconstructViewPos(uv, depth);
    // camera postion is flaot3(0, 0, 0) in view space
    float3 viewDir = normalize(-viewPos);
    float3 reflectionDir = reflect(viewDir, normal);
    
    float2 hitUV;
    float3 hitViewSpace = RayMarchSSR(viewPos, reflectionDir, hitUV, depth);
   
    float3 reflectColor = float3(0.0, 0.0, 0.0);
    if (hitUV.x >= 0.f)
    {
        reflectColor = screenColor.SampleLevel(samLinear_clamp, hitUV, 0).xyz;
    }
    float4 orm = screenORM.SampleLevel(samLinear_clamp, uv, 0);
    float metallic = orm.b; 
    float roughness = orm.g;
    float reflectivity = saturate(pow(1.0 - roughness, 2.0)) * metallic;
    float3 color = screenColor.SampleLevel(samLinear_clamp, uv, 0).xyz;
    float3 finalColor = color + (reflectColor * reflectivity);
    return float4(finalColor, 1.f);
}