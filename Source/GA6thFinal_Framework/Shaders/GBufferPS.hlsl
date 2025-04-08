struct PS_INPUT
{
    float4 position         : SV_POSITION;
    float3 worldPosition    : POSITION0;
    float4 shadowPosition   : POSITION1;
    float3 normal           : NORMAL;
    float3 tangent          : TANGENT;
    float3 biTangent        : BITANGENT;
    float2 uv               : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 diffuse          : SV_Target0;
    float4 normal           : SV_Target1;
    float4 RMA              : SV_Target2;
    float4 emissive         : SV_Target3;
    float4 shadowPosition   : SV_Target4;
    uint   layerMask        : SV_Target5;
};

cbuffer LayerMask : register(b0)
{
    uint layerMask;
}

Texture2D txDiffuse          : register(t0);
Texture2D txNormal           : register(t1);
Texture2D txARM              : register(t2);
Texture2D txEmissive         : register(t3);
Texture2D txOpacity          : register(t4);

SamplerState samLinear_wrap : register(s0);

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT)0;
    
    // diffuse
    output.diffuse = txDiffuse.Sample(samLinear_wrap, input.uv);
    
    // normal
    float3 normal = txNormal.Sample(samLinear_wrap, input.uv).rgb;
    float3x3 TBN = float3x3(input.tangent, input.biTangent, input.normal);
    output.normal = float4(normalize(mul(normal * 2.0f - 1.0f, TBN)), 1);
    
    // pbr
    float3 arm = txARM.Sample(samLinear_wrap, input.uv).rgb;
    output.RMA.r = arm.g;
    output.RMA.g = arm.b;
    output.RMA.b = arm.r;

    // emissive
    output.emissive = txEmissive.Sample(samLinear_wrap, input.uv);   
    
    // shadow
    output.shadowPosition = input.shadowPosition;
    
    // layerMask
    output.layerMask = layerMask;
    
    return output;
}
