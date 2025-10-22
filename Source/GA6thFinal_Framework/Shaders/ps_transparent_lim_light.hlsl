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

struct RimData
{
    float3 color;
    float power;
    float intensity;
};

ConstantBuffer<RimData> bit32_5_rimData;
Texture2D textures[];

PSOutput ps_main(PSInput input)
{
    uint normalID = input.materialID[NORMAL];

    float3 T = input.tangent;
    float3 B = input.biTangent;
    float3 N = input.normal;
    OrthonormalizeTBN(T, B, N);
    float3x3 TBN = float3x3(T, B, N);   

    float3 normal = textures[normalID].Sample(samLinear_wrap, input.uv).rgb;
    if (length(normal))
    {    
        normal = normalize(normal * 2.0f - 1.0f);
        normal = normalize(mul(normal, TBN));
    }
    else
    {
        normal = N;
    }
    
    float3 viewPos = cameraData.Position.xyz;        
    float3 worldPosition = input.worldPosition.xyz;
    float3 V = normalize(viewPos - worldPosition);
    
    RimData data = bit32_5_rimData;
    
    float NdotV = saturate(dot(normal, V));    
    float rim = pow(1.0f - NdotV, data.power);
    
    float fresnel = pow(1.0f - NdotV, 5.0f);
    rim = saturate(rim + fresnel * 0.2f);
    rim = smoothstep(0.0f, 1.0f, rim);
    
    float3 color = data.color * data.intensity;

    PSOutput output = (PSOutput) 0;
    output.color = float4(color, rim);
    output.normal = float4(normal, 1);
    output.depth = input.position.z;
    output.customDepth = input.customDepth;

    return output;
}