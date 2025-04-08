#include "ShaderUtilities.hlsli"
#include "Lighting.hlsli"

struct PS_INPUT
{
    float4 position         : SV_POSITION;
    float3 worldPosition    : POSITION0;
    float4 shadowPosition   : POSITION1;
    float3 normal           : NORMAL;
    float3 tangent          : TANGENT;
    float3 biTangent        : BITANGENT;
    float2 uv               : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 color     : SV_Target0;
    float4 ambient   : SV_Target1;
    float4 normal    : SV_target2;
    uint   layerMask : SV_target3;
};

cbuffer LayerMask
{
    uint layerMask;
}

cbuffer RimLightDesc
{
    
};

Texture2D txDiffuse          : register(t0);
Texture2D txNormal           : register(t1);
Texture2D txARM              : register(t2);
Texture2D txEmissive         : register(t3);
Texture2D txOpacity          : register(t4);

#define PBR
#define IBL
#define Shadow

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT)0;
    
    float4 opacity = txOpacity.Sample(samLinear_wrap, input.uv);
    clip(opacity.a < 0.1f ? -1 : 1);
    
    float3 normalTex = txNormal.Sample(samLinear_wrap, input.uv).xyz;
    float3x3 TBN = float3x3(input.tangent, input.biTangent, input.normal);
    float3 N = normalize(mul(normalTex * 2.0f - 1.0f, TBN));
    
    float3 albedo = GammaToLinearSpace(txDiffuse.Sample(samLinear_wrap, input.uv).rgb);
    float  alpha = 1.0;
    
    float3 directLighting = 0;
    float3 ambientLighting = 0;
    float  shadowFactor = 1.0;
    
    float3 V = normalize(cameraPosition - input.worldPosition);

#ifdef PBR
    float3 ARM = txARM.Sample(samLinear_wrap, input.uv).rgb;
    
    // PBR_Directional
    directLighting += DirectionalLightPBR(input.worldPosition, N, V, albedo, ARM.g, ARM.b);

    // PBR_Point
    directLighting += PointLightPBR(input.worldPosition, N, V, albedo, ARM.g, ARM.b);
#endif
    
#ifdef IBL        
    AmbientIBL ambientIBL = AmbientLightIBL(input.uv, N, V, albedo, ARM.g, ARM.b, ARM.r);
    //ambientIBL.diffuseIBL.rgb = LinearToGammaSpace(ambientIBL.diffuseIBL.rgb);
    ambientLighting = ambientIBL.diffuseIBL.rgb * ambientIBL.diffuseIBL.a + ambientIBL.specularIBL;
#endif
   
#ifdef Shadow
    float currentShadowDepth = input.shadowPosition.z / input.shadowPosition.w;
    float2 uv = input.shadowPosition.xy / input.shadowPosition.w;    
    uv.y = -uv.y;
    uv = uv * 0.5 + 0.5;
    
    if (uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1)
    {
        float2 offsets[9] =
        {
            float2(-1, -1), float2(0, -1), float2(1, -1),
            float2(-1, 0), float2(0, 0), float2(1, 0),
            float2(-1, 1), float2(0, 1), float2(1, 1)
        };
        
        shadowFactor = 0;
    
        [unroll]
        for (int i = 0; i < 9; i++)
        {
            float2 sampleUV = uv + offsets[i] * texel;
            float bias = 0.001f;
            shadowFactor += txShadowDepth.SampleCmpLevelZero(samComparison, sampleUV, currentShadowDepth - bias);
        }
        
        shadowFactor = 1;
    }    
    
#endif
    
    float4 color = 0;
    color.rgb = shadowFactor * directLighting + ambientLighting;
    color.a = alpha;            
    
    if (1 > opacity.a)
    {
        color.a = opacity.a;
        ambientIBL.diffuseIBL.rgb = opacity.a;
    }
    else
        color.a = 1.f;
    
    color.rgb = LinearToGammaSpace(color.rgb);
    //color.rgb += RimLight(N, V);
    float4 emissive = txEmissive.Sample(samLinear_wrap, input.uv);
    color += emissive;
    
    output.color = color;
    output.layerMask = layerMask;
    output.normal = float4(N, 1);
    output.ambient = ambientIBL.diffuseIBL;
    
    return output;
}