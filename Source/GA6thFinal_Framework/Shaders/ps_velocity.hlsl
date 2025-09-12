cbuffer CameraData
{
    float4x4 CurrView;
    float4x4 CurrProj_NoJitter; // non-jitter
    float4x4 PrevView;
    float4x4 PrevProj_NoJitter; // non-jitter
    uint2 RenderSize; // renderW, renderH
    float2 pad;
};

struct PSInput
{
    float4 posH : SV_POSITION; // jittered for rasterization
    float4 currClipNJ : TEXCOORD0; // current non-jitter clip
    float4 prevClip : TEXCOORD1; // previous non-jitter clip
};

float2 ClipToScreen(float4 clip)
{
    float2 ndc = clip.xy / clip.w; // [-1,1]
    float2 ss = float2(0.5, -0.5) * ndc + 0.5; // [0,1], D3D 좌표
    return ss;
}

float2 ToPixels(float2 ssCurr, float2 ssPrev, uint2 renderSize)
{
    return (ssCurr - ssPrev) * float2(renderSize);
}

float2 PackMotion(float2 motionPixels)
{
    return motionPixels; // R=dx(px), G=dy(px)
}

float2 SaturateBig(float2 v, float maxAbs)
{
    return clamp(v, -maxAbs, maxAbs);
}

float2 MotionForInvalid(float2 defaultValue)
{
    return defaultValue;
}

float2 ComputeObjectMotion(PSInput input)
{
    float2 currSS = ClipToScreen(input.currClipNJ);
    float2 prevSS = ClipToScreen(input.prevClip);
    float2 motion = ToPixels(currSS, prevSS, RenderSize);
    return motion;
}

float2 ps_main(PSInput input) : SV_Target0
{
    // 가려짐/하늘/알파 객체 등은 마스크 처리 고려
    float2 motion = ComputeObjectMotion(input);
    // 안전을 위해 클램프
    motion = SaturateBig(motion, 2048.0);
    return PackMotion(motion);
}