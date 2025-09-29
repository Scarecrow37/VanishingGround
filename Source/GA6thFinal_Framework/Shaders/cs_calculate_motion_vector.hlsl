#include "Function.hlsli"

Texture2D<float> depthMap;
RWTexture2D<float2> motionVector;
ConstantBuffer<SSGIData> SSGIDatas;

[numthreads(16,16,1)]
void cs_main(uint3 tid : SV_DispatchThreadID)
{
    uint2 px = tid.xy;
    
    // 화면 밖 픽셀 스킵
    if (px.x >= (uint) SSGIDatas.ScreenSize.x || px.y >= (uint) SSGIDatas.ScreenSize.y)
        return;
    
    float2 uv = (float2(px) + 0.5f) / SSGIDatas.ScreenSize;
    float depth = depthMap.Load(int3(px, 0)).r;
    
    float3 worldPos = ReconstructWorldPos(uv, depth, SSGIDatas.InverseViewProjection);
    float4 prevClip = mul(float4(worldPos, 1.f), SSGIDatas.PreViewProj);
    if(prevClip.w ==0.f)
    {
        motionVector[px] = float2(0, 0);
        return;
    }
    float2 prevNDC = prevClip.xy / prevClip.w;
    prevNDC.y = -prevNDC.y;
    float2 prevUV = prevNDC * 0.5f + 0.5f;
    float2 currUV = uv;
    float2 motionUV = currUV - prevUV;
    motionVector[px] = motionUV;
}