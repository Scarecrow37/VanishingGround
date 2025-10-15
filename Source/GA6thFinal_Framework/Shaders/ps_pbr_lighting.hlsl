#include "Function.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

cbuffer bit32_1_isssao
{
    uint UseSSAO;
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
    
    float ssao = 1;
    if (1==UseSSAO)
        ssao = SSAOMap.SampleLevel(samLinear_wrap, input.uv, 0).r;
    float3 viewPos = cameraData.Position.xyz;
    
    float4 NDC = float4(input.uv * 2.0 - 1, depth, 1.0);
    NDC.y = -NDC.y;
    float4 homogeneous = mul(NDC, cameraData.ProjectionInverse);
    homogeneous = mul(homogeneous, cameraData.ViewInverse);
    float3 worldPosition = homogeneous.xyz / homogeneous.w;
    float3 V = normalize(viewPos - worldPosition);
    
    float3 directLighting = float3(0, 0, 0);
    float3 ambientLighting = 0;
    float3 ambient = CalculateIBL(normal, V, irradianceMap, prefilteredMap, brdfLUT, albedo, roughness, metallic);
    
    NumLight numLights = bit32_4_numLight;
    
    //Directional Lights
    for (uint i = 0; i < numLights.Directional; i++)
    {
        DirectionalLight light = lightData.Directional[i];
        
        float shadow = CalculateShadow(worldPosition, normal, light.Direction, shadowMap);
        directLighting += CalculateDirectional(light, normal, V, albedo, metallic, roughness) * shadow;
        ambientLighting += ambient * light.Ambient;
    }
    
    //Point Lights
    for (uint j = 0; j < numLights.Point; j++)
    {
        PointLight light = lightData.Point[j];
        directLighting += CalculatePoint(light, normal, V, albedo, metallic, roughness, worldPosition);
    }
    
    //Spot Lights
    for (uint k = 0; k < numLights.Spot; k++)
    {
        SpotLight light = lightData.Spot[k];
        directLighting += CalculateSpot(light, normal, V, albedo, metallic, roughness, worldPosition);
    }
    
    //Shadow Point Lights
    for (uint l = 0; l < numLights.ShadowPoint; l++)
    {
        PointLight light = lightData.ShadowPoint[l];
        directLighting += CalculatePoint(light, normal, V, albedo, metallic, roughness, worldPosition);
    }
    

    float3 color = directLighting + (ambientLighting * ssao) + emissive;
    return float4(color, 1.0);
}