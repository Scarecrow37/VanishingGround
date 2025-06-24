#include "Function.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
};


struct GbufferID
{
    uint BaseColor;
    uint Normal;
    uint Orm;
    uint Emissive;
    uint WorldPosition;
    uint Depth;
    uint CumstomDepth;
};

ConstantBuffer<GbufferID> bit32_7_gBufferID;
Texture2D textures[];

#define BASECOLOR bit32_7_gBufferID.BaseColor
#define NORMAL bit32_7_gBufferID.Normal
#define ORM bit32_7_gBufferID.Orm
#define EMISSIVE bit32_7_gBufferID.Emissive
#define WORLDPOSITION bit32_7_gBufferID.WorldPosition
#define DEPTH bit32_7_gBufferID.Depth
#define CUMSTOMDEPTH bit32_7_gBufferID.CumstomDepth

float4 ps_main(PSInput input) : SV_Target0
{
    float depth = textures[DEPTH].Sample(samLinear_wrap, input.uv).r;
    float3 albedo = textures[BASECOLOR].Sample(samLinear_wrap, input.uv).rgb;
    clip(1.f - Epsilon - depth);
    albedo = GammaToLinearSpace(albedo);
    //pow(albedo, 2.2);
    
    float3 normal = textures[NORMAL].Sample(samLinear_wrap, input.uv).rgb;
   
    float3 orm = textures[ORM].Sample(samLinear_wrap, input.uv).rgb;
    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;
    
    float3 viewPos = cameraData.Position.xyz;
    float3 fragPos = textures[WORLDPOSITION].Sample(samLinear_wrap, input.uv).xyz;
    float3 V = normalize(viewPos - fragPos);
    
    float3 diffuse = float3(0, 0, 0);
        
    //Directional Lights
    for (uint i = 0; i < numLight.Directional; i++)
    {
        DirectionalLight light = lightData.Directional[i];
        diffuse += CalculateDirectional(light, normal, V, albedo, metallic, roughness);
    }
    //Point Lights
    for (uint j = 0; j < numLight.Point; j++)
    {
        PointLight light = lightData.Point[j];                
        diffuse += CalculatePoint(light, normal, V, albedo, metallic, roughness, fragPos);
    }
    
    //Spot Lights
    for (uint k = 0; k < numLight.Spot; k++)
    {
        SpotLight light = lightData.Spot[k];
        diffuse += CalculateSpot(light, normal, V, albedo, metallic, roughness, fragPos);
    }
    
    // ibl specular brdf 따로 구현하기
    // 임시로 albedo 색상에 상수값으로 환경광 표현
    float3 ambient = 0.3;
    ambient *= albedo;
    //diffuse = pow(diffuse, 1.0 / 2.2);
    float3 color = diffuse;//+ambient;

    return float4(color, 1.0);
}