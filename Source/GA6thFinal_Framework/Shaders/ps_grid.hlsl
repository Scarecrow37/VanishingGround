#include "CommonData.hlsli"

struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
};

float ComputeGridAlpha(float2 worldXZ, float gridStep, float alpha)
{
    float2 gridUV = worldXZ / gridStep;
    float2 grid = abs(frac(gridUV - 0.5) - 0.5) / fwidth(gridUV);
    float lines = min(grid.x, grid.y);
    float lineAlpha = exp(-lines * lines);
    return lineAlpha * alpha;
}

float4 ps_main(PSInput input) : SV_TARGET
{
    float2 worldXZ = input.worldPos.xz;

    // ----- 카메라 높이 기반 LOD 계산 -----
    float camHeight = max(cameraData.Position.y, 1);
    float logH = log10(camHeight);

    float baseStep = pow(10.0, floor(logH)); // ex: 10, 100, ...
    float nextStep = baseStep * 10.0;

    float t = saturate(logH - floor(logH)); // 페이드 비율: 0~1

    // 선 색상
    float3 color = float3(1.0, 1.0, 1.0);

    // LOD 단계별 알파 계산 (보간)
    float alphaLow = (1.0 - t);
    float alphaHigh = t;

    // 라인 투명도 계산 (작은 그리드 + 큰 그리드)
    float lineAlphaLow = ComputeGridAlpha(worldXZ, baseStep, alphaLow);
    float lineAlphaHigh = ComputeGridAlpha(worldXZ, nextStep, alphaHigh);

    float finalAlpha = saturate(lineAlphaLow + lineAlphaHigh);

    // 거리 페이드
    float dist = distance(cameraData.Position.xz, worldXZ);
    float fade = saturate(1.0 - dist / 1500.0);
    
    return float4(color, finalAlpha * fade);
}