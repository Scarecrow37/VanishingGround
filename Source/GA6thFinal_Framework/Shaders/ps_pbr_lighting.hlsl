#include "Function.hlsli"
struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
};

#define BASECOLOR 0
#define NORMAL 1
#define ORM 2
#define EMISSIVE 3
#define WORLDPOSITION 4
#define DEPTH 5

ConstantBuffer<NumLight> num_light : register(b0);
StructuredBuffer<DirectionalLight> directionalLights;
StructuredBuffer<PointLight> pointLights;
StructuredBuffer<SpotLight> spotLights;
Texture2D gBuffers[];
SamplerState samLinear_wrap;


float4 ps_main(PSInput input) : SV_Target0
{
    float depth = gBuffers[DEPTH].Sample(samLinear_wrap, input.uv).r;
    float3 albedo = gBuffers[BASECOLOR].Sample(samLinear_wrap, input.uv).rgb;
    clip(1.f - Epsilon - depth);
    albedo = pow(albedo, 2.2);
    
    float3 normal = gBuffers[NORMAL].Sample(samLinear_wrap, input.uv).rgb;
   
    float3 orm = gBuffers[ORM].Sample(samLinear_wrap, input.uv).rgb;
    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;
    
    float3 viewPos = cameraData.Position.xyz;
    float3 fragPos = gBuffers[WORLDPOSITION].Sample(samLinear_wrap, input.uv).xzy;
    float3 V = normalize(viewPos - fragPos);
    
    float3 diffuse = float3(0, 0, 0);
    
    //Directional Lights
    for (uint i = 0; i < /*num_light.Directional*/1;++i)
    {
        DirectionalLight light = directionalLights[i];
        diffuse += CalculateDirectional(light, normal, V, albedo, metallic, roughness);
    }
    //일단 빛 구현 안되어있으니 사용안함
    //Point Lights
    //for (uint j = 0; j < num_light.Point;++j)
    //{
    //    PointLight light = pointLights[j];
    //    diffuse += CalculatePoint(light, normal, V, albedo, metallic, roughness, fragPos);
    //}
    ////Spot Lights
    //for (uint k = 0; k < num_light.Spot; ++k)
    //{
    //    SpotLight light = spotLights[k];
    //    diffuse += CalculateSpot(light, normal, V, albedo, metallic, roughness, fragPos);
    //}
    
    // ibl specular brdf 따로 구현하기
    diffuse = pow(diffuse, 1.0 / 2.2);
    float3 color = diffuse; //+ specular;

    return float4(color, 1.0);
}