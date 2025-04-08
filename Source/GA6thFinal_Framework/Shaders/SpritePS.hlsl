struct PS_INPUT
{
    float4 position      : SV_POSITION;
    float2 uv            : TEXCOORD0;
};

cbuffer SpriteAlpha
{
    float alpha;
};

Texture2D txSprite           : register(t0);
SamplerState samLinear_wrap  : register(s0);

float4 main(PS_INPUT input) : SV_Target
{
    float4 color = txSprite.Sample(samLinear_wrap, input.uv);
    color.a *= alpha;
    return color;
}