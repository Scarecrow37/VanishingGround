#include "Function.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2DArray shadowMap;
TextureCube irradianceMap;
TextureCube prefilteredMap;
Texture2D brdfLUT;
Texture2D baseColorMap;
Texture2D normalMap;
Texture2D ormMap;
Texture2D emissiveMap;
Texture2D depthMap;
Texture2D customDepthMap;
Texture2D<float> SSAOMap;

//Texture2D textures[];
//
//#define BASECOLOR 0
//#define NORMAL 1
//#define ORM 2
//#define EMISSIVE 3
//#define WORLDPOSITION 4
//#define DEPTH 5
//#define CUMSTOMDEPTH 6

float4 ps_main(PSInput input) : SV_Target
{
    float depth = depthMap.Sample(samLinear_wrap, input.uv).r;
    clip(1.f - Epsilon - depth);
    
    float3 albedo = baseColorMap.Sample(samLinear_wrap, input.uv).rgb;
    albedo = GammaToLinearSpace(albedo);

    float3 emissive = emissiveMap.Sample(samLinear_wrap, input.uv).rgb;    
    emissive = GammaToLinearSpace(emissive);
    
    float3 normal = normalMap.Sample(samLinear_wrap, input.uv).rgb;

    float3 orm = ormMap.Sample(samLinear_wrap, input.uv).rgb;
    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;
    
    float ssao = SSAOMap.SampleLevel(samLinear_wrap, input.uv,0).r;
    float3 viewPos = cameraData.Position.xyz;
    
    float4 NDC = float4(input.uv * 2.0 - 1, depth, 1.0);
    NDC.y = -NDC.y;
    float4 homogeneous = mul(NDC, cameraData.ProjectionInverse);
    homogeneous = mul(homogeneous, cameraData.ViewInverse);
    float3 worldPosition = homogeneous.xyz / homogeneous.w;
    float3 V = normalize(viewPos - worldPosition);

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
    float3 ambient = CalculateIBL(normal, V, irradianceMap, prefilteredMap, brdfLUT, albedo, roughness, metallic);
    
    //Directional Lights
    for (uint i = 0; i < numLight.Directional; i++)
    {
        DirectionalLight light = lightData.Directional[i];
        
        float shadow = CalculateShadow(worldPosition, normal, light.Direction, shadowMap);
        directLighting += CalculateDirectional(light, normal, V, albedo, metallic, roughness) * shadow;
        ambientLighting += ambient * light.Ambient;
    }
    
    //Point Lights
    for (uint j = 0; j < numLight.Point; j++)
    {
        PointLight light = lightData.Point[j];
        directLighting += CalculatePoint(light, normal, V, albedo, metallic, roughness, worldPosition);
    }
    
    //Spot Lights
    for (uint k = 0; k < numLight.Spot; k++)
    {
        SpotLight light = lightData.Spot[k];
        directLighting += CalculateSpot(light, normal, V, albedo, metallic, roughness, worldPosition);
    }

    float3 color = directLighting + (ambientLighting * ssao) + emissive;
    return float4(color, 1.0);
}