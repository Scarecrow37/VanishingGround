#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
};

struct PSOutput
{
    float4 normal        : SV_Target0;
    float depth          : SV_Target1;
    uint customDepth     : SV_Target2;
};

#define NORMAL 0

struct Material
{
    uint ID[4];
};

StructuredBuffer<Material> material;
Texture2D textures[];

float3 CalculateNormal(float3 sampledNormal, float3 tangent, float3 bitangent, float3 normal)
{
    sampledNormal = normalize(sampledNormal * 2.0 - 1.0);
    float3x3 TBN = float3x3(tangent, bitangent, normal);
    return normalize(mul(sampledNormal, TBN));
}

PSOutput WriteGuBuffer(PSInput input)
{
    PSOutput output = (PSOutput) 0;
    
    //ObjectData data = bit32_4_objectData;
    
    //uint normalID = material[data.ID].ID[NORMAL];
    
    ////output.baseColor.rgb = pow(output.baseColor.rgb, 2.2);
    //// 1. normal
    //float3 normal = textures[normalID].Sample(samLinear_wrap, input.uv).xyz;
    //normal = CalculateNormal(normal, input.tangent, input.biTangent, input.normal);
    //output.normal = float4(normal, 1.f);
    
    ////5. depth
    //output.depth = input.position.z;
    //// SWTODO : 나중에 마스킹값 받는거 처리
    //output.customDepth = data.CustomDepth;
    
    return output;
}

PSOutput ps_main(PSInput input)
{
    PSOutput output = (PSOutput) 0;
    output = WriteGuBuffer(input);

    return output;
}
