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
};


Texture2D screenColor;
Texture2D screenNormal;
Texture2D screenDepth;
Texture2D screenORM;
ConstantBuffer<SSRProperty> bit32_3_ssrProperty;

float noise(float2 seed)
{
    return frac(sin(dot(seed.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float4 RayTrace(float3 reflectDirVS, float3 viewPos, float2 uv)
{
    SSRProperty property = bit32_3_ssrProperty;
    float4 color = float4(0.0, 0.0, 0.0, 0.0);
    float3 step = property.stepSize * reflectDirVS;

    float jitter = noise(uv + 40.0f);
    int maxRayCount = (int) property.maxRayCount;

    [loop]
    for (int i = 0; i <= maxRayCount; ++i)
    {
        float3 ray = (float(i) + jitter) * step;
        float3 rayPosVS = viewPos + ray;

        // View → Clip → UV
        float4 clipPos = mul(cameraData.Projection, float4(rayPosVS, 1.0f));
        if (clipPos.w <= 0.0001f)
            continue;

        float2 rayUV = clipPos.xy / clipPos.w * 0.5f + 0.5f;
        rayUV.y = 1.0f - rayUV.y;

        if (rayUV.x < 0.0f || rayUV.x > 1.0f || rayUV.y < 0.0f || rayUV.y > 1.0f)
            continue;

        float sampleDepth = screenDepth.SampleLevel(samLinear_clamp, rayUV, 0).r;

        // reconstruct view space Z from NDC depth
        float3 sampleViewPos = ReconstructViewPos(rayUV, sampleDepth);

        float depthDiff = rayPosVS.z - sampleViewPos.z;
        if (depthDiff > 0.0f && depthDiff < property.maxThickness)
        {
            float a = 0.3f * pow(min(1.0f, (property.stepSize * maxRayCount / 2) / length(ray)), 2.0f);
            color = color * (1.0f - a) + float4(screenColor.SampleLevel(samLinear_clamp, rayUV, 0).rgb, 1.0f) * a;
            break;
        }
    }

    return color;
}

float4 ps_main(PSInput input) : SV_Target
{
    float3 color = screenColor.SampleLevel(samLinear_clamp, input.uv, 0).xyz;
    uint width, height;
    screenColor.GetDimensions(width, height);
    
    float depth = screenDepth.SampleLevel(samLinear_clamp, input.uv, 0).r;
    
    if(depth >= 1.0f) 
        return float4(color, 1.0f);
    
    float3 normal = screenNormal.SampleLevel(samLinear_clamp, input.uv, 0).xyz;
    float3 viewnormal = mul((float3x3) cameraData.View, normal);
    float3 viewPos = ReconstructViewPos(input.uv, depth);
    float3 viewDir = normalize(-viewPos);
    float3 reflectDir = normalize(reflect(viewDir, viewnormal));
    
    float4 reflectedColor = RayTrace(reflectDir, viewPos, input.uv);
    
    float4 orm = screenORM.SampleLevel(samLinear_clamp, input.uv, 0);
    float metallic = orm.b; 
    float roughness = orm.g;
    float reflectivity = lerp(0.04, 1.0, metallic); // 금속 여부에 따라
    float reflectionWeight = reflectivity * (1.0 - roughness * roughness); // 조절식
    return float4(color + (reflectedColor.rgb * reflectionWeight),1.f);
    //return float4(color + reflectedColor.rgb, 1.f);
}