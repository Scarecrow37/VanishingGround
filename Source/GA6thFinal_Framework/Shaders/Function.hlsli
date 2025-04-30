#include "CommonData.hlsli"

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.f - F0) * pow(1.f - cosTheta, 5);
}

float NormalDistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.f) + 1.f);
    
    return a2 / (PI * denom * denom + Epsilon);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness = 1.f;
    float k = (r * r) / 8.f;
    
    return NdotV / (NdotV * (1.f - k) + k + Epsilon);
}

float GeometrySmith(float3 N, float V, float3 L, float roughness)
{
    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
    
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

float3 DiffuseBRDF(float3 N, float3 V, float3 L, float3 albedo, float metallic, float roughness)
{
    float3 H = normalize(V + L);
    float NdotL = saturate(dot(N, L));
    float VdotH = saturate(dot(V, H));
    
    float3 F0 = lerp(float3(Fdielectric, Fdielectric, Fdielectric), albedo, metallic);
    float3 F = FresnelSchlick(VdotH, F0);
    float3 D = NormalDistributionGGX(N, H, roughness);
    float3 G = GeometrySmith(N, V, L, roughness);
    
    float3 specular = (D * G * F) / max(4.0 * saturate(dot(N, V)) * NdotL, Epsilon);
    float3 kd = (1.0 - F) * (1.0 - metallic);
    float3 diffuse = kd * albedo / PI;

    return (diffuse + specular) * NdotL;
}

float3 CalculateDirectional(DirectionalLight light,float3 N, float3 V,float3 albedo,float metallic,float roughness)
{
    //이거 사용해야하고 빛 구현 안되어있으니깐 임시 데이터 사용
    //float3 L = normalize(-light.direction).xyz;
    //float3 radiance = light.strength.rgb;
    float3 L = float3(0, 0, -1);
    float3 radiance = float3(1.0, 1.0, 1.0);
    return DiffuseBRDF(N, V, L, albedo, metallic, roughness) * radiance;
}

float3 CalculatePoint(PointLight light,float3 N, float3 V,float3 albedo,float metallic,float roughness,float3 fragPos)
{
    float3 L = light.position - fragPos;
    float distance = length(L);
    L = normalize(L);
    float3 attenuation = 1.f / (light.fallOffEnd + distance * distance);
    float3 radiance = light.strength.rgb * attenuation;
        
    return DiffuseBRDF(N, V, L, albedo, metallic, roughness) * radiance;
}

float3 CalculateSpot(SpotLight light,float3 N,float3 V, float3 albedo,float metallic,float roughness,float3 fragPos)
{
    float3 L = light.position - fragPos;
    float distance = length(L);
    L = normalize(L);

    float theta = dot(-L, normalize(light.direction.xyz));
    float epsilon = light.spotInnerCone - light.spotOuterCone;
    float intensity = saturate((theta - light.spotOuterCone) / max(epsilon, 1e-4));

    float attenuation = 1.0 / (light.fallOffEnd + distance * distance);
    float3 radiance = light.strength * attenuation * intensity;
    return DiffuseBRDF(N, V, L, albedo, metallic, roughness) * radiance;
}