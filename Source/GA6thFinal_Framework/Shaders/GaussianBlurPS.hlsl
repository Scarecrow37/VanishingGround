#include "PostProcess.hlsli"

#define SAMPLING_COUNT 9
    
static float weights[SAMPLING_COUNT] =
{
    0.013519569015984728,
    0.047662179108871855,
    0.11723004402070096,
    0.20116755999375591,
    0.240841295721373,
    0.20116755999375591,
    0.11723004402070096,
    0.047662179108871855,
    0.013519569015984728
};

Texture2D txSource1 : register(t2);

float4 main(PS_INPUT input) : SV_Target
{
    float indices[SAMPLING_COUNT] = { -4, -3, -2, -1, 0, +1, +2, +3, +4 };
    
#ifdef X
    float2 direction = float2(1.0, 0.0);
#else
    float2 direction = float2(0.0, 1.0);
#endif
    
    float2 offset = 1 / screenSize;
    float2 step = direction * offset;
    float4 result = 0.0f;
    
    [unroll]
    for (int i = 0; i < SAMPLING_COUNT; i++)
    {
        result += txSource1.Sample(samLinear_wrap, float2(input.uv + indices[i] * step)) * weights[i];
    }
    
    return float4(result.rgb, 1);
}