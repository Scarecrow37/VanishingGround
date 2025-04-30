#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
};

#define Fdielectric  0.04
#define BASECOLOR 0
#define NORMAL 1
#define ORM 2
#define EMISSIVE 3
#define WORLDPOSITION 4

struct DirectionalLight
{
    float4 direction;
    float4 strength;
};

struct PointLight
{
    float3 position;
    float fallOffEnd;
    float4 strength;
};

struct SpotLight
{
    float3 position;
    float fallOffEnd;
    float4 direction;
    float spotInnerCone;
    float spotOuterCone;
    float2 padding;
};

struct NumLight
{
    uint Directional;
    uint Point;
    uint Spot;
    uint padding;
};

ConstantBuffer<NumLight> num_light : register(b0);
StructuredBuffer<DirectionalLight> directionalLights;
StructuredBuffer<PointLight> pointLights;
StructuredBuffer<SpotLight> spotLights;
Texture2D gBuffers[];
SamplerState samLinear_wrap;

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.f - F0) * pow(1.f - cosTheta, 5);
}

float NormalDistributionGGX(float3 N,float3 H,float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = saturate(dot(N, H));
    float NdotH2 = NdotH * NdotH;
    float denom = (NdotH2 * (a2 - 1.f) + 1.f);
    
    return a2 / (PI * denom * denom + Epsilon);
}

float GeometrySchlickGGX(float NdotV,float roughness)
{
    float r = roughness = 1.f;
    float k = (r * r) / 8.f;
    
    return NdotV / (NdotV * (1.f - k) + k + Epsilon);
}

float GeometrySmith(float3 N, float V,float3 L,float roughness)
{
    float NdotV = saturate(dot(N, V));
    float NdotL = saturate(dot(N, L));
    
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

float3 BRDF(float3 N, float3 V,float3 L,float3 albedo,float metallic,float roughness)
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


float4 ps_main(PSInput input) : SV_Target0
{
    float3 albedo = gBuffers[BASECOLOR].Sample(samLinear_wrap, input.uv).rgb;
    albedo = pow(albedo, 2.2);
    
    float3 normal = gBuffers[NORMAL].Sample(samLinear_wrap, input.uv).rgb;
   
    float3 orm = gBuffers[ORM].Sample(samLinear_wrap, input.uv).rgb;
    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;
    
    float3 viewPos = cameraData.Position.xyz;
    float3 fragPos = gBuffers[WORLDPOSITION].Sample(samLinear_wrap, input.uv).xzy;
    float3 V = normalize(viewPos - fragPos);
    
    float3 Lo = float3(0, 0, 0);
    
    //Directional Lights
    for (uint i = 0; i < num_light.Directional;++i)
    {
        DirectionalLight light = directionalLights[i];
        float3 L = normalize(-light.direction).xyz;
        float3 radiance = light.strength.rgb;
        Lo += BRDF(normal, V, L, albedo, metallic, roughness) * radiance;
    }
    //Point Lights
    for (uint i = 0; i < num_light.Point;++i)
    {
        PointLight light = pointLights[i];
        float3 L = light.position - fragPos;
        float distance = length(L);
        L = normalize(L);
        float3 attenuation = 1.f / (light.fallOffEnd + distance * distance);
        float3 radiance = light.strength.rgb * attenuation;
        
        Lo += BRDF(normal, V, L, albedo, metallic, roughness) * radiance;
    }
    //Spot Lights
    

}