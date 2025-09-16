#include "Function.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    float4 worldPosition : TEXCOORD1;
};

struct PSOutput
{
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
    float depth : SV_Target2;
    uint customDepth : SV_Target3;
};

#define DIFFUSE   0
#define NORMAL    1
#define ORM       2
#define EMISSIVE  3

struct Material
{
    uint ID[4];
};

StructuredBuffer<Material> material;
ConstantBuffer<GbufferData> bit32_2_gbufferData;
Texture2DArray shadowMap;
TextureCube irradianceMap;
TextureCube prefilteredMap;
Texture2D brdfLUT;
Texture2D<float> SSAOMap;
Texture2D textures[];

PSOutput ps_main(PSInput input)
{    
    TranslucentObjectData data = bit32_4_translucentObjectData;
    
    uint diffuseID = material[data.ID].ID[DIFFUSE];
    uint normalID = material[data.ID].ID[NORMAL];
    uint ORMID = material[data.ID].ID[ORM];
    uint emissiveID = material[data.ID].ID[EMISSIVE];
    
    float mimBias = bit32_2_gbufferData.MipBias;
    float alpha = data.Alpha;
    
    float4 albedo = textures[diffuseID].SampleBias(samLinear_wrap, input.uv, mimBias);
    
    clip(albedo.a * alpha - Epsilon);
    albedo.rgb = GammaToLinearSpace(albedo.rgb);
    
    float3 T = input.tangent;
    float3 B = input.biTangent;
    float3 N = input.normal;
    OrthonormalizeTBN(T, B, N);
    float3x3 TBN = float3x3(T, B, N);

    float3 emissive = textures[emissiveID].SampleBias(samLinear_wrap, input.uv, mimBias).rgb;
    emissive = GammaToLinearSpace(emissive);

    float3 normal = textures[normalID].SampleBias(samLinear_wrap, input.uv, mimBias).rgb;
    normal = normalize(normal * 2.0f - 1.0f);
    normal = normalize(mul(normal, TBN));

    float3 orm = textures[ORMID].SampleBias(samLinear_wrap, input.uv, mimBias).rgb;
    float ao = orm.r;
    float roughness = orm.g;
    float metallic = orm.b;
    
    float ssao = SSAOMap.SampleLevel(samLinear_wrap, input.uv, 0).r;
    float3 viewPos = cameraData.Position.xyz;
        
    float3 worldPosition = input.worldPosition.xyz;
    float3 V = normalize(viewPos - worldPosition);
    
    float3 directLighting = float3(0, 0, 0);
    float3 ambientLighting = 0;
    float3 ambient = CalculateIBL(normal, V, irradianceMap, prefilteredMap, brdfLUT, albedo.rgb, roughness, metallic);
    
    NumLight numLights = bit32_3_numLight;
    //Directional Lights
    for (uint i = 0; i < numLights.Directional; i++)
    {
        DirectionalLight light = lightData.Directional[i];
        
        float shadow = CalculateShadow(worldPosition, normal, light.Direction, shadowMap);
        directLighting += CalculateDirectional(light, normal, V, albedo.rgb, metallic, roughness) * shadow * 0.001;
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

    float3 color = directLighting + (ambientLighting * ssao) + emissive;
    
    PSOutput output = (PSOutput) 0;
    output.color = float4(color, albedo.a * alpha);
    output.normal = float4(normal, 1);
    output.depth = input.position.z;
    output.customDepth = data.CustomDepth;
    
    return output;
}