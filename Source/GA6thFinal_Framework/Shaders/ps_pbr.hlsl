#include "CommonData.hlsli"

float3 FresnelFactor(float cosTheta, float3 F0)
{
    return F0 + (1 - F0) * pow(1 - cosTheta, 5);
}

float NormalDistributionFunction(float3 N, float3 Lh, float roughness)
{
    float alpha = pow(roughness, 2);
    float alphaSq = alpha * alpha;
    float denominator = PI * pow(pow(dot(N, Lh), 2) * (alphaSq - 1) + 1, 2);
    return alphaSq / denominator;
}
float GSchlickGGX(float3 Vector, float3 N, float roughness)
{
    float NDotVec = saturate(dot(Vector, N));
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return NDotVec / (NDotVec * (1.0 - k) + k);
}
float GAFDirect(float3 N, float3 Lo, float3 Li, float roughness)
{
    return GSchlickGGX(Lo, N, roughness) * GSchlickGGX(Li, N, roughness);
}

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    float4 worldPosition : POSITION;
};

#define Fdielectric  0.04

#define DIFFUSE 0
#define NORMAL 1
#define ORM 2
#define EMISSIVE 3

struct Material
{
    uint ID[4];
};

ConstantBuffer<Object> bit32_object : register(b1);
StructuredBuffer<Material> material;

#define object bit32_object

Texture2D textures[];
SamplerState samLinear_wrap;

float4 ps_main(PSInput input) : SV_Target
{
    float3 tempLight = float3(0, 0, 1);
    
    uint diffuseID = material[object.ID].ID[DIFFUSE];
    uint normalID = material[object.ID].ID[NORMAL];
    uint ORMID = material[object.ID].ID[ORM];
    uint emissiveID = material[object.ID].ID[EMISSIVE];
    
    float3 albedo = textures[diffuseID].Sample(samLinear_wrap, input.uv).xyz;
    albedo = pow(albedo, 2.2);
    float3 Normal = textures[normalID].Sample(samLinear_wrap, input.uv).xyz;
    Normal = normalize(Normal);
    Normal = (Normal * 2.f) - 1.f;
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    input.biTangent = normalize(input.biTangent);
    float3x3 TBN = float3x3(input.tangent, input.biTangent, input.normal);
    float3 N = normalize(mul(Normal, TBN));
   
    float ao = textures[ORMID].Sample(samLinear_wrap, input.uv).r;
    float roughness = textures[ORMID].Sample(samLinear_wrap, input.uv).b;
    float metallic = textures[ORMID].Sample(samLinear_wrap, input.uv).g;
    
    float3 f0 = lerp(Fdielectric, albedo, metallic);
    float3 directLighting = 0.f;
    float3 ambientLighting = 1.f;
    
    float3 L = -normalize(tempLight);
    float3 V = normalize(cameraData.Position.xyz - input.worldPosition.xyz);
    float NdotV = max(0,dot(N, V));
    float3 H = normalize(L + V);
    float NdotL = max(0, dot(N, L));
    float NdotH = max(0, dot(N, H));
    
    float3 F = FresnelFactor(max(0.0, dot(H, V)), f0);
    float D = NormalDistributionFunction(N, H, max(0.01, roughness));
    float G = GAFDirect(N, V, L, roughness);
    float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
    
    // PBR
    float3 diffuseBRDF = kd * albedo / 3.141592;
    float3 specularBRDF = (F * D * G) / max(0.0001, (4 * NdotL * NdotV));
    directLighting += (diffuseBRDF + specularBRDF) * NdotL;
    // TODO : ibl ????????????
    ambientLighting = ambientLighting * ao;
 
    float alpha = textures[diffuseID].Sample(samLinear_wrap, input.uv).a;
    
    float4 finalColor;
    finalColor = float4(directLighting , alpha);
    finalColor.xyz = pow(finalColor.xyz, 1 / 2.2);
    //return float4(albedo, 1.f);
    return finalColor;
}