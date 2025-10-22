#include "Function.hlsli"

ConstantBuffer<SSGIData> ssgiData;

Texture2D<float> screenDepth;
Texture2D<float4> screenNormal;
Texture2D<float4> screenAlbedo;
Texture2D<float4> screenColor;
Texture2D<float4> screenORM;

RWTexture2D<float4> SSGI_Result;

#define PI 3.14159265
#define TWO_PI (PI * 2.0)
#define HALF_PI (PI * 0.5)

float randf(int x, int y)
{
    return frac(sin(dot(float2(x, y), float2(12.9898, 78.233))) * 43758.5453);
}

[numthreads(16, 16, 1)]
void cs_main(uint3 dtid : SV_DispatchThreadID)
{
    int2 pixel = int2(dtid.xy);
    float2 uv = (pixel * 2.f + 1.f) / ssgiData.ScreenSize;

    float depth = screenDepth.SampleLevel(samPoint_clamp, uv, 0);
    if (depth >= 1.0f)
    {
        SSGI_Result[pixel] = float4(0, 0, 0, 0);
        return;
    }

    // ===== 기본 데이터 복원 =====
    float3 viewPos = ReconstructViewPos(uv, depth);
    float3 normal = normalize(screenNormal.SampleLevel(samPoint_clamp, uv, 0).xyz);
    float3 albedo = screenAlbedo.SampleLevel(samPoint_clamp, uv, 0).xyz;
    float3 directLight = screenColor.SampleLevel(samPoint_clamp, uv, 0).xyz;

    float2 metalRough = screenORM.SampleLevel(samPoint_clamp, uv, 0).gb;
    float metallic = metalRough.y;
    float roughness = max(0.05, metalRough.x);
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo, metallic);

    float3 V = normalize(-viewPos);

    // ===== GI 샘플 누적 =====
    float3 giLighting = 0.0.xxx;

    float Nd = 4.0;
    float Ns = 8.0;
    float sliceRotation = TWO_PI / (Nd - 1.0);
    float sampleScale = (-ssgiData.Radius * cameraData.Projection[0][0]) / viewPos.z;
    float jitter = 0.f; //randf(pixel.x, pixel.y) - 0.5;

    [loop]
    for (float slice = 1.0; slice <= Nd; slice += 1.0)
    {
        float phi = sliceRotation * (slice + jitter) + PI;
        float2 omega = float2(cos(phi), sin(phi));
        float2 aspect = ssgiData.ScreenSize.yx / ssgiData.ScreenSize.x;

        [loop]
        for (float s = 1.0; s <= Ns; s += 1.0)
        {
            float stepScale = s / Ns;
            float2 sampleUV = uv - stepScale * sampleScale * omega * aspect;
            if (any(sampleUV < 0.0) || any(sampleUV > 1.0))
                continue;

            float sDepth = screenDepth.SampleLevel(samPoint_clamp, sampleUV, 0);
            if (sDepth >= 1.0f)
                continue;

            float3 sampleViewPos = ReconstructViewPos(sampleUV, sDepth);
            float3 sampleNormal = normalize(screenNormal.SampleLevel(samPoint_clamp, sampleUV, 0).xyz);
            float3 sampleLight = screenColor.SampleLevel(samPoint_clamp, sampleUV, 0).xyz;

            float3 L = normalize(sampleViewPos - viewPos);
            float dist = length(sampleViewPos - viewPos);
            if (dist > ssgiData.Radius || dist < 0.001)
                continue;

            float NdotL = saturate(dot(normal, L));
            if (NdotL <= 0.0)
                continue;

            float3 H = normalize(V + L);
            float NdotV = saturate(dot(normal, V));
            float NdotH = saturate(dot(normal, H));
            float VdotH = saturate(dot(V, H));

            float D = GeometrySchlickGGX(NdotH, roughness);
            float G = GeometrySmith(normal, V, L, roughness);
            float3 F = FresnelSchlick(VdotH, F0);

            float3 kS = F;
            float3 kD = (1.0 - kS) * (1.0 - metallic);
            float3 specular = (D * G * F) / max(4.0 * NdotV * NdotL + 0.001, 0.001);
            float3 brdf = (kD * albedo / PI + specular) * NdotL;

            giLighting += sampleLight * brdf;
        }
    }

    giLighting /= (Nd * Ns);

    float3 result = giLighting * ssgiData.Intensity;
    
    SSGI_Result[pixel] = float4(result, 1.0);
}
