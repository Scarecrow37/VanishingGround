struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv       : TEXCOORD;
};

struct PS_OUTPUT
{
    float4 color     : SV_Target0;
    uint   layerMask : SV_target3;
};

cbuffer WaveData
{
    float waveTime;
    float waveAmplitude;
    float waveFrequency;
}

cbuffer LayerMask
{
    uint layerMask;
}

Texture2D txBase        : register(t0);
Texture2D txMask        : register(t1);
Texture2D txGradient    : register(t2);

SamplerState samLinear_wrap : register(s0);
SamplerState samLinear_clamp : register(s1);

float random(float2 uv)
{
    return frac(sin(dot(uv.xy, float2(12.9898, 78.233))) * 43758.5453);
}

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0;
    
    float base = txBase.Sample(samLinear_wrap, input.uv + float2(waveTime, 0)).r;
    float3 mask = txMask.Sample(samLinear_wrap, input.uv).rgb;
    float noise = random(input.uv * waveTime * 10.0);
    
    float3 baseColor = txGradient.Sample(samLinear_wrap, input.uv).rgb;
    float4 color = float4(baseColor * mask + baseColor, 1);
    color.a = base;
    
    output.color = color;
    output.layerMask = layerMask;
    
    return output;
}