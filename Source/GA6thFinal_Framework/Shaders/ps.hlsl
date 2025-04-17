struct PSInput
{
    float4 position     : SV_POSITION;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 biTangent    : BINORMAL;
    float2 uv           : TEXCOORD;
};

Texture2D texDiffuse : register(t0);
Texture2D texNormal  : register(t1);
Texture2D texGrass   : register(t2);
Texture2D texMask    : register(t3);

SamplerState samLinear_wrap;

float4 ps_main(PSInput input) : SV_TARGET
{
    float3 tempLight = float3(0, -1, 1);
    
    float4 diffuse = texDiffuse.Sample(samLinear_wrap, input.uv);
    float3 Noraml = texNormal.Sample(samLinear_wrap, input.uv).xyz * 2 - 1;
    float mask = texMask.Sample(samLinear_wrap, input.uv).x;
    
    diffuse = diffuse * (1 - mask);
    
    float3 L = -normalize(tempLight);
    float3x3 TBN = float3x3(input.tangent, input.biTangent, input.normal);
    float3 N = mul(Noraml, TBN);
    
    float NdotL = max(0, dot(N, L));

    float3 grass = texGrass.Sample(samLinear_wrap, input.uv).xyz;    
    float4 color = saturate((diffuse + float4(grass * mask, 1)) * NdotL);
    
    return color;
}