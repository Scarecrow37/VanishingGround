Texture2D screen : register(t0);
Texture2D UI : register(t1);
SamplerState samLinear_wrap;

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
};

float4 ps_main(PSInput input) : SV_Target
{
    float4 screenPixel = screen.Sample(samLinear_wrap, input.uv);
    
    return screenPixel;
}