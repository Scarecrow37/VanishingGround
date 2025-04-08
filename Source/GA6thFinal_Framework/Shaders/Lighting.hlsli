#ifndef __Lighting__
#define __Lighting__

#include "Constants.hlsli"
#include "LightData.hlsli"

inline float CalculatePointLightIntensity(float distance, float range, float intensity, float3 K)
{
    //if (range < distance)
    //    return 0;
    
    // float attenuation = 1.0 / (K.x + K.y * distance + K.z * distance * distance);
    // return intensity * attenuation;
    float attenuation = saturate(1.0f - (distance * distance / (range * range)));
    return intensity * attenuation * attenuation;
}

inline float NormalDistribution(float roughness, float3 N, float3 H)
{
    float alpha = roughness * roughness;
    float squareA = alpha * alpha;
    float NdotH = max(0.0, dot(N, H));
    float denom = NdotH * NdotH * (squareA - 1.0) + 1.0;
    return squareA / (PI * denom * denom);
}

inline float3 FresnelReflection(float costTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - costTheta, 5.0);
}

inline float GSchlickGGX(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

inline float GeometricAttenuation(float3 N, float3 L, float3 V, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // direct
    //float k = (roughness * roughness) * 0.5; // IBL
    return GSchlickGGX(max(0.0, dot(N, V)), k) * GSchlickGGX(max(0.0, dot(N, L)), k);
}

inline float3 DirectionalLightPBR(float3 worldPosition, float3 N, float3 V, float3 albedo, float roughness, float metalness)
{
    float3 directLighting = 0;
    float3 F0 = lerp(Fdielectric, albedo, metalness);
    float NdotV = max(0, dot(N, V));
    
    for (uint i = 0; i < numDirectionalLights; i++)
    {
        Light light = DirectionalLights[i];
        float3 L = -normalize(light.data);
        float3 H = normalize(V + L);
        float3 F = FresnelReflection(max(0, dot(H, V)), F0);
        float  D = NormalDistribution(max(0.01, roughness), N, H);
        float  G = GeometricAttenuation(N, L, V, roughness);
        float3 kd = lerp(1.0 - F, 0, metalness);
        float  NdotL = max(0, dot(N, L));
    
        float3 diffusePBR = albedo * kd / PI * light.diffuse.rgb;
        float3 specularPBR = (F * D * G) / max(0.00001, 4.0 * NdotL * NdotV) * light.specular.rgb;
        
        float intensity = 1.0;
        //float toonStep = 3;
        //intensity = floor(NdotL * toonStep) / toonStep;
        directLighting += (diffusePBR + specularPBR) * intensity * light.intensity;
    }
    
    return directLighting;
}

inline float3 PointLightPBR(float3 worldPosition, float3 N, float3 V, float3 albedo, float roughness, float metalness)
{
    float3 directLighting = 0;
    float3 F0 = lerp(Fdielectric, albedo, metalness);
    float NdotV = max(0, dot(N, V));
    
    for (uint i = 0; i < numPointLights; i++)
    {
        Light light = PointLights[i];
        float3 distance = light.data - worldPosition;
        float len = length(distance);
        float3 L = distance / len;
        float3 H = normalize(V + L);
        float3 F = FresnelReflection(max(0, dot(H, V)), F0);
        float  D = NormalDistribution(max(0.01, roughness), N, H);
        float  G = GeometricAttenuation(N, L, V, roughness);
        float3 kd = lerp(1.0 - F, 0, metalness);
        float  NdotL = max(0, dot(N, L));
    
        float3 diffusePBR = albedo * kd / PI * light.diffuse.rgb;
        float3 specularPBR = (F * D * G) / max(0.00001, 4.0 * NdotL * NdotV) * light.specular.rgb;
        float intensity = CalculatePointLightIntensity(len, light.range, light.intensity, light.attenuation);
        directLighting += (diffusePBR + specularPBR) * NdotL * intensity;
    }
    
    return directLighting;
}

struct AmbientIBL
{
    float4 diffuseIBL;
    float3 specularIBL;
};

inline AmbientIBL AmbientLightIBL(float2 uv, float3 N, float3 V, float3 albedo, float roughness, float metalness, float ao)
{    
    AmbientIBL ambientIBL = (AmbientIBL)0;
    
    float3 F0 = lerp(Fdielectric, albedo, metalness);
    float3 irradiance = txIBL_Diffuse.Sample(samLinear_wrap, N).rgb;
    
    float NdotV = max(0, dot(N, V));
    
    uint width, height, levels;
    txIBL_Specular.GetDimensions(0, width, height, levels);

    float3 Lr = 2.0 * NdotV * N - V;
    float3 preFilteredColor = txIBL_Specular.SampleLevel(samLinear_wrap, Lr, roughness * levels).rgb;
    float2 brdf = txIBL_BRDF.Sample(samLinear_clamp, float2(NdotV, roughness)).rg;
        
    float3 F = FresnelReflection(NdotV, F0);
    float3 kD = lerp(1.0 - F, 0, metalness);
    float3 diffuseIBL = kD * albedo * irradiance;
    float3 specularIBL = (F0 * brdf.x + brdf.y) * preFilteredColor;
    
    ambientIBL.diffuseIBL.rgb = diffuseIBL;
    ambientIBL.diffuseIBL.a = ao;
    ambientIBL.specularIBL = specularIBL;
    
    return ambientIBL;
}

inline float3 RimLight(float3 N, float3 V)
{
    float rim = 1.0 - dot(N, V);
    rim = pow(rim, 1.5);

    return float3(0.0, 0.5, 1.0) * rim;
}

#endif