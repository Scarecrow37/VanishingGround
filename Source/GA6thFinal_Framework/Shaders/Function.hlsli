#include "CommonData.hlsli"

float3 FresnelSchlick(float cosTheta, float3 F0);
float NormalDistributionGGX(float3 N, float3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(float3 N, float3 V, float3 L, float roughness);
float3 DiffuseBRDF(float3 N, float3 V, float3 L, float3 albedo, float metallic, float roughness);
float3 CalculateDirectional(DirectionalLight light, float3 N, float3 V, float3 albedo, float metallic, float roughness);
float3 CalculatePoint(PointLight light, float3 N, float3 V, float3 albedo, float metallic, float roughness, float3 fragPos);
float3 CalculateSpot(SpotLight light, float3 N, float3 V, float3 albedo, float metallic, float roughness, float3 fragPos);
float Attenuation(float3 attenuation, float distance, float range);

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
    float r = roughness + 1;
    float k = (r * r) / 8.f;
    
    return NdotV / (NdotV * (1.f - k) + k + Epsilon);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
    
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

float3 DiffuseBRDF(float3 N, float3 V, float3 L, float3 albedo, float metallic, float roughness)
{
    float3 H = normalize(V + L);
    float  NdotL = saturate(dot(N, L));
    float  VdotH = saturate(dot(V, H));
    
    float3 F0 = lerp(float3(Fdielectric, Fdielectric, Fdielectric), albedo, metallic);
    float3 F = FresnelSchlick(VdotH, F0);
    float3 D = NormalDistributionGGX(N, H, roughness);
    float3 G = GeometrySmith(N, V, L, roughness);
    
    float3 specular = (D * G * F) / max(4.0 * saturate(dot(N, V)) * NdotL, Epsilon);
    float3 kd = (1.0 - F) * (1.0 - metallic);
    float3 diffuse = kd * albedo / PI;

    return (diffuse + specular) * NdotL;
}

float3 CalculateDirectional(DirectionalLight light, float3 N, float3 V, float3 albedo, float metallic, float roughness)
{
    float3 L = -light.Direction;    
    return DiffuseBRDF(N, V, L, albedo, metallic, roughness) * light.Color * light.Intensity;
}

float3 CalculatePoint(PointLight light, float3 N, float3 V, float3 albedo, float metallic, float roughness, float3 fragPos)
{
    float3 L = light.Position - fragPos;
    float distance = length(L);
    L = normalize(L);    
    float attenuation = Attenuation(light.Attenuation, distance, light.Range);
        
    return DiffuseBRDF(N, V, L, albedo, metallic, roughness) * attenuation * light.Color * light.Intensity;
}

float3 CalculateSpot(SpotLight light, float3 N, float3 V, float3 albedo, float metallic, float roughness, float3 fragPos)
{
    float3 L = light.Position - fragPos;
    float distance = length(L);
    L = normalize(L);

    float theta = dot(-L, normalize(light.Direction.xyz));
    float epsilon = light.InnerCone - light.OuterCone;
    float intensity = saturate((theta - light.OuterCone) / max(epsilon, 1e-4)) * light.Intensity;
    float attenuation = Attenuation(light.Attenuation, distance, light.Range);

    return DiffuseBRDF(N, V, L, albedo, metallic, roughness) * attenuation * light.Color * light.Intensity;
}

float Attenuation(float3 attenuation, float distance, float range)
{
    //float result = 1.f / (attenuation.r + attenuation.g * distance + attenuation.b * distance * distance);
    //result *= 2;
    
    float result = saturate(1.0f - (distance * distance / (range * range)));
    result = result * result;

    //// 사이거리 - 광원의 반지름을 구하여 진짜 거리 계산
    //float d = max(distance - range, 0); // 거리가 음수면 0으로 설정

    //// 감쇠 계산을 위한 중간 값 설정 (거리 / 광원의 반지름 + 1)
    //float denom = (d / range) + 1;

    //// 감쇠 강도 계산 (거리가 크면 감쇠되고 반지름이 크면 빛을 많이 받음)
    //float result = 1 / (denom * denom);

    //// 감쇠 스케일 재조정 임계값
    //// att가 0일때는 광원과 가장 멈
    //// att가 1일 때는 광원 중심에 가장 가까움
    //result = (result - Epsilon) / (1 - Epsilon);


    //// att가 음수가 나오지 않도록 조정
    //result = saturate(result);
    
    return result;
}