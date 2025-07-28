#include "Function.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

TextureCube irradianceMap;
TextureCube prefilteredMap;
Texture2D brdfLUT;
Texture2D baseColorMap;
Texture2D normalMap;
Texture2D ormMap;
Texture2D emissiveMap;
Texture2D worldPositionMap;
Texture2D depthMap;
Texture2D customDepthMap;

//Texture2D textures[];
//
//#define BASECOLOR 0
//#define NORMAL 1
//#define ORM 2
//#define EMISSIVE 3
//#define WORLDPOSITION 4
//#define DEPTH 5
//#define CUMSTOMDEPTH 6

float4 ps_main(PSInput input) : SV_Target0
{
    float depth = depthMap.Sample(samLinear_wrap, input.uv).r;
    float3 albedo = baseColorMap.Sample(samLinear_wrap, input.uv).rgb;
    clip(1.f - Epsilon - depth);
    albedo = GammaToLinearSpace(albedo);

    float3 normal = normalMap.Sample(samLinear_wrap, input.uv).rgb;

    float3 orm = ormMap.Sample(samLinear_wrap, input.uv).rgb;
    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;
    
    float3 viewPos = cameraData.Position.xyz;
    float3 fragPos = worldPositionMap.Sample(samLinear_wrap, input.uv).xyz;
    float3 V = normalize(viewPos - fragPos);
    
    //float depth = textures[DEPTH].Sample(samLinear_wrap, input.uv).r;
    //float3 albedo = textures[BASECOLOR].Sample(samLinear_wrap, input.uv).rgb;
    //clip(1.f - Epsilon - depth);
    //albedo = GammaToLinearSpace(albedo);
    
    //float3 normal = textures[NORMAL].Sample(samLinear_wrap, input.uv).rgb;
   
    //float3 orm = textures[ORM].Sample(samLinear_wrap, input.uv).rgb;
    //float ao = orm.r;
    //float roughness = orm.g;
    //float metallic = orm.b;
    
    //float3 viewPos = cameraData.Position.xyz;
    //float3 fragPos = textures[WORLDPOSITION].Sample(samLinear_wrap, input.uv).xyz;
    //float3 V = normalize(viewPos - fragPos);
    
    float3 directLighting = float3(0, 0, 0);
    float3 ambientLighting = 0;
    float3 ambient = CalculateIBL(normal, V, irradianceMap, prefilteredMap, brdfLUT, albedo, roughness, metallic) * ao;
        
    //Directional Lights
    for (uint i = 0; i < numLight.Directional; i++)
    {
        DirectionalLight light = lightData.Directional[i];
        directLighting += CalculateDirectional(light, normal, V, albedo, metallic, roughness);
        ambientLighting += ambient * light.Ambient;
    }
    //Point Lights
    for (uint j = 0; j < numLight.Point; j++)
    {
        PointLight light = lightData.Point[j];                
        directLighting += CalculatePoint(light, normal, V, albedo, metallic, roughness, fragPos);
    }
    
    //Spot Lights
    for (uint k = 0; k < numLight.Spot; k++)
    {
        SpotLight light = lightData.Spot[k];
        directLighting += CalculateSpot(light, normal, V, albedo, metallic, roughness, fragPos);
    }


    float3 color = directLighting + ambientLighting;

    return float4(color, 1.0);
}