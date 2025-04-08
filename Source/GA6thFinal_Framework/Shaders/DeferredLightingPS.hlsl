#include "ShaderUtilities.hlsli"
#include "Lighting.hlsli"

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 color    : SV_Target0;
    float4 ambient  : SV_Target1;
};

Texture2D txDiffuse         : register(t0);
Texture2D txNormal          : register(t1);
Texture2D txRMA             : register(t2);
Texture2D txEmissive        : register(t3);
Texture2D txShadowPosition  : register(t4);
Texture2D txDepth           : register(t5);

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT)0;
    
    float  depth = txDepth.Sample(samLinear_wrap, input.uv).r;
    float3 albedo = GammaToLinearSpace(txDiffuse.Sample(samLinear_wrap, input.uv).rgb);        
    float3 RMA = txRMA.Sample(samLinear_wrap, input.uv).rgb;
    
    float4 NDC = float4(input.uv * 2.0 - 1, depth, 1.0);
    NDC.y = -NDC.y;
    float4 homogeneous = mul(NDC, viewprojInv);
    float3 worldPosition = homogeneous.xyz / homogeneous.w;
    
    float3 N = txNormal.Sample(samLinear_wrap, input.uv).xyz;
    float3 V = normalize(cameraPosition - worldPosition);
    
    float3 directLighting = 0;
    float3 ambientLighting = 0;
    float  shadowFactor = 1.0;
    
// PBR
    // PBR_Directional
    directLighting += DirectionalLightPBR(worldPosition, N, V, albedo, RMA.r, RMA.g);;

    // PBR_Point
    directLighting += PointLightPBR(worldPosition, N, V, albedo, RMA.r, RMA.g);
    
// IBL
    AmbientIBL ambientIBL = AmbientLightIBL(input.uv, N, V, albedo, RMA.r, RMA.g, RMA.b);
    //ambientIBL.diffuseIBL.rgb = LinearToGammaSpace(ambientIBL.diffuseIBL.rgb);
    ambientLighting = ambientIBL.diffuseIBL.rgb * ambientIBL.diffuseIBL.a + ambientIBL.specularIBL;
    
//Shadow
    float4 shadowPosition = txShadowPosition.Sample(samLinear_wrap, input.uv);
    float currentShadowDepth = shadowPosition.z / shadowPosition.w;
    float2 uv = shadowPosition.xy / shadowPosition.w;
    
    uv.y = -uv.y;
    uv = uv * 0.5 + 0.5;
    
    if (uv.x >= 0 && uv.x <= 1 && uv.y >= 0 && uv.y <= 1)
    {
        shadowFactor = 0;
        float bias = 0.001;
        
        [unroll]
        for (int i = 0; i < 9; i++)
        {
            float2 sampleUV = uv + offsets[i] * texel;
            shadowFactor += txShadowDepth.SampleCmpLevelZero(samComparison, sampleUV, currentShadowDepth - bias);
        }
        
        shadowFactor = shadowFactor / 9.0;
    }   
    
    output.color.rgb = shadowFactor * directLighting + ambientLighting;
    output.color.rgb = LinearToGammaSpace(output.color.rgb);
    output.color.a = 1;
    
    float4 emissive = txEmissive.Sample(samLinear_wrap, input.uv);
    output.color += emissive;
    
    output.ambient = ambientIBL.diffuseIBL;
    
    return output;
}