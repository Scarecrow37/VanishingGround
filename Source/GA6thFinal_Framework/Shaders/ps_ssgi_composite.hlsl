#include "CommonData.hlsli"
#include "Function.hlsli"

Texture2D<float4> screenColor;
Texture2D<float4> ssgiTexture;
Texture2D<float4> screenAlbedo;

struct PSInput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

float4 ps_main(PSInput input):SV_Target
{
    float4 screen = screenColor.SampleLevel(samLinear_clamp, input.uv, 0);
    float4 ssgi = ssgiTexture.SampleLevel(samLinear_clamp, input.uv, 0);
    float4 albedo = screenAlbedo.SampleLevel(samLinear_clamp, input.uv, 0);
    
    // 간접광
    float3 indirectLighting = ssgi.rgb * albedo.rgb;
    float3 final = screen.rgb + indirectLighting;
    return float4(final, 1.f);
}