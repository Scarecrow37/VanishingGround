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
    float4 baseColor : SV_Target0;
    float4 normal : SV_Target1;
    float4 orm : SV_Target2;
    float4 emissive : SV_Target3;
    float depth : SV_Target4;
    uint costomDepth : SV_Target5;
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

PSOutput ps_main(PSInput input)
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
    normal = (normal * 2.f) - 1.f;
    normal = normalize(normal);
    float3 T = input.tangent;
    float3 B = input.biTangent;
    float3 N = input.normal;
    float3x3 TBN = float3x3(T, B, N);
    normal = normalize(mul(normal, TBN));
    output.normal = float4(normal, 1.f);
    //2. ORM
    float ao = textures[ORMID].Sample(samLinear_wrap, input.uv).r;
    float roughness = textures[ORMID].Sample(samLinear_wrap, input.uv).g;
    float metallic = textures[ORMID].Sample(samLinear_wrap, input.uv).b;
    output.orm = float4(ao, roughness, metallic, 1.f);
    output.emissive = textures[emissiveID].Sample(samLinear_wrap, input.uv);
    output.depth = input.position.z;
    // SWTODO : 나중에 마스킹값 받는거 처리
    output.costomDepth = 0;
    
    return output;
}