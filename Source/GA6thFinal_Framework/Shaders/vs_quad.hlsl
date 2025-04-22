struct VSInput
{
    float4 position : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biTangent : BINORMAL;
    float2 uv : TEXCOORD;
};

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    output.position = float4(input.position.xyz, 1.f);
    output.uv = input.uv;
    return output;
}