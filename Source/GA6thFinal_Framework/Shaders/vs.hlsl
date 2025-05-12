struct DefaultConstantBuffer
{
    matrix world;
    matrix view;
    matrix projection;
    matrix WV;
};

ConstantBuffer<DefaultConstantBuffer> transform : register(b0);

struct VSInput
{
    float4 position     : POSITION;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 biTangent    : BINORMAL;
    float2 uv           : TEXCOORD;
};

struct VSOutput
{
    float4 position     : SV_POSITION;
    float3 normal       : NORMAL;
    float3 tangent      : TANGENT;
    float3 biTangent    : BINORMAL;
    float2 uv           : TEXCOORD;
};

VSOutput vs_main(VSInput input)
{
    VSOutput output = (VSOutput) 0;

    output.position = mul(input.position, transform.world);
    output.position = mul(output.position, transform.view);
    output.position = mul(output.position, transform.projection);

    output.normal = mul(input.normal, (float3x3) transform.world);
    output.tangent = mul(input.tangent, (float3x3) transform.world);
    output.biTangent = mul(input.biTangent, (float3x3) transform.world);
    
    output.uv = input.uv;

    return output;
}