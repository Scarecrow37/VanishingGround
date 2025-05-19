#include "CommonData.hlsli"

struct PSInput
{
    float4 position     : SV_POSITION;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 biTangent    : BINORMAL;
    float2 uv           : TEXCOORD;
};

#define DIFFUSE 0
#define NORMAL 1
#define MASK 2
#define GRASS 3

struct Material
{
    uint ID[4];
};


StructuredBuffer<Material> material;

Texture2D textures[];

float4 ps_main(PSInput input) : SV_TARGET
{
    float3 tempLight = float3(0, 0, 1);
    
    uint diffuseID = material[object.ID].ID[DIFFUSE];
    uint normalID = material[object.ID].ID[NORMAL];
    uint maskID = material[object.ID].ID[MASK];
    uint grassID = material[object.ID].ID[GRASS];
        
    float4 diffuse = textures[diffuseID].Sample(samLinear_wrap, input.uv);
    float3 Noraml = textures[normalID].Sample(samLinear_wrap, input.uv).xyz * 2 - 1;
    float mask = textures[maskID].Sample(samLinear_wrap, input.uv).x;
    
    //diffuse = diffuse * (1 - mask);
    
    float3 L = -normalize(tempLight);
    float3x3 TBN = float3x3(input.tangent, input.biTangent, input.normal);
    float3 N = mul(Noraml, TBN);
    
    float NdotL = max(0, dot(N, L));

    float3 grass = textures[grassID].Sample(samLinear_wrap, input.uv).xyz;
    float4 color = saturate((diffuse + float4(grass * mask, 1)) * NdotL);
    
    //color = diffuse;
    return color;
}