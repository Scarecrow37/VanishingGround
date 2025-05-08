#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
    float4 worldPosition : POSITION;
};

struct PSOutput
{
    float4 baseColor     : SV_Target0;
    float4 normal        : SV_Target1;
    float4 orm           : SV_Target2;
    float4 emissive      : SV_Target3;
    float4 worldPosition : SV_Target4;
    float depth          : SV_Target5;
    uint customDepth     : SV_Target6;
};

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


float3 CalculateNormal(float3 sampledNormal, float3 tangent, float3 bitangent, float3 normal)
{
    sampledNormal = normalize(sampledNormal * 2.0 - 1.0);
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    return normalize(mul(sampledNormal, TBN));
}

PSOutput WriteGuBuffer(PSInput input)
{
    PSOutput output = (PSOutput) 0;
    uint diffuseID = material[object.ID].ID[DIFFUSE];
    uint normalID = material[object.ID].ID[NORMAL];
    uint ORMID = material[object.ID].ID[ORM];
    uint emissiveID = material[object.ID].ID[EMISSIVE];
    
    // 0. baseColor
    output.baseColor = textures[diffuseID].Sample(samLinear_wrap, input.uv);
    //output.baseColor.rgb = pow(output.baseColor.rgb, 2.2);
    // 1. normal
    float3 normal = textures[normalID].Sample(samLinear_wrap, input.uv).xyz;
    normal = CalculateNormal(normal, input.tangent, input.biTangent, input.normal);
    output.normal = float4(normal, 1.f);
    //2. ORM
    float ao = textures[ORMID].Sample(samLinear_wrap, input.uv).r;
    float roughness = textures[ORMID].Sample(samLinear_wrap, input.uv).g;
    float metallic = textures[ORMID].Sample(samLinear_wrap, input.uv).b;
    output.orm = float4(ao, roughness, metallic, 1.f);
    //3. emissive
    output.emissive = textures[emissiveID].Sample(samLinear_wrap, input.uv);
    //4. worldPosition
    output.worldPosition = input.worldPosition;
    //5. depth
    output.depth = input.position.z;
    // SWTODO : 나중에 마스킹값 받는거 처리
    output.customDepth = 0;
    return output;
}

PSOutput ps_main(PSInput input)
{
    PSOutput output = (PSOutput) 0;
    output = WriteGuBuffer(input);

    return output;
}
