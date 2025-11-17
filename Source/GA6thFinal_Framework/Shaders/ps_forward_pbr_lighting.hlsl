#include "Function.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    float4 worldPosition : TEXCOORD1;
    
    nointerpolation uint4 materialID : TEXCOORD2;
    nointerpolation uint customDepth : TEXCOORD3;
    nointerpolation float alpha : TEXCOORD4;
};

#define DIFFUSE   0
#define NORMAL    1
#define ORM       2
#define EMISSIVE  3

ConstantBuffer<GbufferData> bit32_2_gbufferData;
Texture2DArray shadowMap;
Texture2D pointLightShadowMap;

TextureCube irradianceMap;
TextureCube prefilteredMap;
Texture2D brdfLUT;
Texture2D textures[];

ForwardPSOutput ps_main(PSInput input)
{        
    uint diffuseID = input.materialID[DIFFUSE];
    uint normalID = input.materialID[NORMAL];
    uint ORMID = input.materialID[ORM];
    uint emissiveID = input.materialID[EMISSIVE];
    
    float mimBias = bit32_2_gbufferData.MipBias;
    float alpha = input.alpha;

    float3 T = input.tangent;
    float3 B = input.biTangent;
    float3 N = input.normal;
    OrthonormalizeTBN(T, B, N);
    float3x3 TBN = float3x3(T, B, N);
    
    float4 albedo = textures[diffuseID].SampleBias(samLinear_wrap, input.uv, mimBias);      
    albedo.rgb = GammaToLinearSpace(albedo.rgb);

    float3 emissive = textures[emissiveID].SampleBias(samLinear_wrap, input.uv, mimBias).rgb;
    emissive = GammaToLinearSpace(emissive);

    float3 normal = textures[normalID].SampleBias(samLinear_wrap, input.uv, mimBias).rgb;
    normal = normalize(normal * 2.0f - 1.0f);
    normal = normalize(mul(normal, TBN));

    float3 orm = textures[ORMID].SampleBias(samLinear_wrap, input.uv, mimBias).rgb;
    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;
    
    float3 viewPos = cameraData.Position.xyz;
        
    float3 worldPosition = input.worldPosition.xyz;
    float3 V = normalize(viewPos - worldPosition);
    
    float3 directLighting = float3(0, 0, 0);
    float3 ambientLighting = 0;
    float3 ambient = CalculateIBL(normal, V, irradianceMap, prefilteredMap, brdfLUT, albedo.rgb, roughness, metallic);   
    
    NumLight numLights = bit32_4_numLight;
    //Directional Lights
    for (uint i = 0; i < numLights.Directional; i++)
    {
        DirectionalLight light = lightData.Directional[i];
        
        float shadow = CalculateShadow(worldPosition, normal, light.Direction, shadowMap);
        directLighting += CalculateDirectional(light, normal, V, albedo.rgb, metallic, roughness) * shadow;
        ambientLighting += ambient * light.Ambient;
    }
    
    //Point Lights
    for (uint j = 0; j < numLights.Point; j++)
    {
        PointLight light = lightData.Point[j];
        directLighting += CalculatePoint(light, normal, V, albedo.rgb, metallic, roughness, worldPosition);
    }
    
    //Spot Lights
    for (uint k = 0; k < numLights.Spot; k++)
    {
        SpotLight light = lightData.Spot[k];
        directLighting += CalculateSpot(light, normal, V, albedo.rgb, metallic, roughness, worldPosition);
    }
    
    //Shadow Point Lights
    for (uint l = 0; l < numLights.ShadowPoint; l++)
    {
        PointLight light = lightData.ShadowPoint[l];
        
        float shadow = CalculatePointLightShadowPCF(
                 worldPosition,
                 light.Position,
                 l,
                 pointLightShadowMap,
                 light.Range,
                 8192.0,
                 1024.0
             );
        directLighting += CalculatePoint(light, normal, V, albedo.rgb, metallic, roughness, worldPosition) * shadow;
    }
    float3 color = directLighting + (ambientLighting * ao) + emissive;
    
    ForwardPSOutput output = (ForwardPSOutput) 0;
    output.color = float4(color, albedo.a * alpha);
    output.normal = float4(normal, 1);
    output.customDepth = input.customDepth;

    return output;
}