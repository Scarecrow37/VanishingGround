#include "Compute.hlsli"

StructuredBuffer<ParticleOutput> InPoints;
StructuredBuffer<uint> ConcatIdx; // 이어붙인 인덱스들 
StructuredBuffer<uint> EmitOffsets; // emitter별 시작 위치 0 , 20, 35, ...
RWStructuredBuffer<ParticleOutput> OutPoints;
ConstantBuffer<TessParams> bit32_4_tessellationParams;



[numthreads(128, 1, 1)]
void cs_main(uint3 DTid : SV_DispatchThreadID)
{
    uint tid = DTid.x;
    uint tessFactor = bit32_4_tessellationParams.TessFactor;
    uint emitterCount = bit32_4_tessellationParams.EmitterCount;
    uint emitterIdx = 0;
    for (emitterIdx = 0; emitterIdx < emitterCount; ++emitterIdx)
    {
        if (tid < EmitOffsets[emitterIdx + 1] && tid >= EmitOffsets[emitterIdx])
        {
            break;
        }
    }
    
    for (uint e = 0; e < emitterCount; ++e)
    {
        uint inBegin = EmitOffsets[e];
        uint inEnd = EmitOffsets[e + 1];
        uint inCount = inEnd - inBegin; // Top/Bottom 쌍 포함된 수

        if (inCount < 2)
            continue; // 보간 불가

        uint anchorCount = inCount / 2;
        uint outCount = (anchorCount - 2) * (tessFactor + 1) + 2;
        //uint outStart = EmitStarts[e];

        // tid가 outCount 범위 넘어가면 스킵
        if (tid >= outCount * 2)
            continue;

        // ----- 보간 진행 -----
        uint row = tid % 2; // 0=Top, 1=Bottom
        uint idx = tid / 2; // anchor 인덱스 기준

        // Anchor 구간/로컬 t
        uint seg = idx / (tessFactor + 1);
        uint localIdx = idx % (tessFactor + 1);
        float t = (tessFactor == 0) ? 0.0 : (float) localIdx / (float) (tessFactor + 1);

        // CatmullRom sample anchors
        uint a0 = max(int(seg) - 1, 0);
        uint a1 = seg;
        uint a2 = min(seg + 1, anchorCount - 1);
        uint a3 = min(seg + 2, anchorCount - 1);

        // 각 row별로 인덱스
        uint idx0 = ConcatIdx[inBegin + a0 * 2 + row];
        uint idx1 = ConcatIdx[inBegin + a1 * 2 + row];
        uint idx2 = ConcatIdx[inBegin + a2 * 2 + row];
        uint idx3 = ConcatIdx[inBegin + a3 * 2 + row];

        ParticleOutput P0 = InPoints[idx0];
        ParticleOutput P1 = InPoints[idx1];
        ParticleOutput P2 = InPoints[idx2];
        ParticleOutput P3 = InPoints[idx3];

        // center 보간 (Top 기준으로, Bottom은 동일 center 공유)
        float3 center = CatmullRom(t, P0.Position.xyz, P1.Position.xyz, P2.Position.xyz, P3.Position.xyz);

        // Top/Bottom 분기
        float3 finalPos = center + ((row == 0) ? +side * width : -side * width);

        // outIndex
        uint outIndex = outStart + tid;

        ParticleOutput outP;
        outP = P1; // 기본 복사

        outP.Position.xyz = finalPos;
        // w는 그대로
        outP.Color = lerp(P1.Color, P2.Color, t);
        outP.FrameInfo.z = lerp(P1.FrameInfo.z, P2.FrameInfo.z, t);

        OutPoints[outIndex] = outP;
    }
}
