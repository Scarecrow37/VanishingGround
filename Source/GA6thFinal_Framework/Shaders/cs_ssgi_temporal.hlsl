#include "Function.hlsli"

ConstantBuffer<SSGIData> ssgiData;
Texture2D<float4> prevHalf;
Texture2D<float4> currHalf;
Texture2D<float2> motionVector;

RWTexture2D<float4> temporalHalf;

[numthreads(16,16,1)]
void cs_main(uint3 dtid : SV_DispatchThreadID)
{
    uint halfW = (uint) ceil(ssgiData.ScreenSize.x * 0.5);
    uint halfH = (uint) ceil(ssgiData.ScreenSize.y * 0.5);
    if (dtid.x >= halfW || dtid.y >= halfH)
        return;

    float2 uvHalf = (float2(dtid.xy) + 0.5) / float2(halfW, halfH);
 
    float2 uvFull = uvHalf; 
    float2 motionUV = motionVector.SampleLevel(samLinear_clamp, uvFull, 0).xy;

    float2 prevUVHalf = uvHalf - motionUV;

    float4 curr = currHalf.SampleLevel(samLinear_clamp, uvHalf, 0);
    float4 prev = float4(0, 0, 0, 0);
    if (prevUVHalf.x >= 0.0 && prevUVHalf.x <= 1.0 && prevUVHalf.y >= 0.0 && prevUVHalf.y <= 1.0)
        prev = prevHalf.SampleLevel(samLinear_clamp, prevUVHalf, 0);

    bool validPrev = prev.a > 0.5;
    float4 outColor = curr;
    if (validPrev)
        outColor.rgb = lerp(curr.rgb, prev.rgb, ssgiData.TemporalWeight);

    temporalHalf[dtid.xy] = outColor;
}