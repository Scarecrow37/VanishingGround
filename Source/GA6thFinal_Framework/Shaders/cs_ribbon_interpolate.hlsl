// cs_ribbon_interpolate.hlsl
// 입력: (Top, Bottom, Top, Bottom, ...)
// 출력: 세그먼트마다 TessFactor * 2 (Top, Bottom)
// 규칙:
//  - 센터라인 C: Cardinal/Hermite (tension 조절 가능, 기본 0.15)로 곡선 보간
//  - 폭 W: 선형 보간 + 최소값 클램프
//  - 방향 S: Paddings 그대로 보간(+정규화) + 세그먼트 단위 플립가드
//  - 최종 위치: Top = C + S*W, Bottom = C - S*W
//  - Position.w는 입력 유지
//  - 모든 계산에 EPS 가드로 NaN 방지

#include "Compute.hlsli" // ParticleOutput 등 공용 정의 포함

// 원본 포인트 버퍼(시뮬 결과): (Top,Bottom,Top,Bottom,…)
StructuredBuffer<ParticleOutput> InPoints;

// emitter별 이어붙인 인덱스(위 InPoints의 인덱스)
StructuredBuffer<uint> ConcatIdx;

// emitter별 concat 시작 오프셋 (길이 = emitterCount + 1, 마지막은 가드)
StructuredBuffer<uint> EmitOffsets;

// 출력 버퍼에서 emitter별 시작 오프셋
StructuredBuffer<uint> EmitStarts;

// 출력 보간 포인트
RWStructuredBuffer<ParticleOutput> OutPoints;

// 32-bit 상수
struct TessParams
{
    uint TessFactor; // 세그먼트 내 샘플 수
    uint TotalSegments; // Σ_e max(0, anchors_e-1)  (anchors = 쌍 수 = N/2)
    uint _pad0;
    uint _pad1;
};
ConstantBuffer<TessParams> bit32_4_tessellationParams;

// ===============================
// Hermite(카디널) 스플라인 보간
// tension=0 → Catmull과 유사, 0.1~0.25 추천
// ===============================
static float3 HermiteCardinal(float3 p0, float3 p1, float3 p2, float3 p3, float t, float tension)
{
    // 분모 없음 → NaN 위험 적고 코너에서 연속 곡률
    float s = (1.0f - tension) * 0.5f;

    // 중앙 두 점(p1,p2)에서의 접선
    float3 m1 = s * (p2 - p0);
    float3 m2 = s * (p3 - p1);

    float t2 = t * t;
    float t3 = t2 * t;

    // Cubic Hermite basis
    float h00 = 2.0f * t3 - 3.0f * t2 + 1.0f;
    float h10 = t3 - 2.0f * t2 + t;
    float h01 = -2.0f * t3 + 3.0f * t2;
    float h11 = t3 - t2;

    return h00 * p1 + h10 * m1 + h01 * p2 + h11 * m2;
}

[numthreads(128, 1, 1)]
void cs_main(uint3 DTid : SV_DispatchThreadID)
{
    const uint segId = DTid.x;
    if (segId >= bit32_4_tessellationParams.TotalSegments)
        return;

    // 어떤 emitter/세그먼트인지 찾기
    uint e = 0;
    uint accumSeg = 0;

    while (true)
    {
        const uint off0 = EmitOffsets[e];
        const uint off1 = EmitOffsets[e + 1];
        const uint N = (off1 > off0) ? (off1 - off0) : 0; // 원본 포인트 수(Top,Bottom,…)
        const uint anchors = (N >= 2) ? (N >> 1) : 0; // 쌍(Top+Bottom) 수
        const uint segCountE = (anchors > 1) ? (anchors - 1) : 0;

        if (segId < accumSeg + segCountE)
        {
            const uint segInE = segId - accumSeg;

            // 이 세그먼트의 (Top0,Bottom0) / (Top1,Bottom1)
            const uint iTop0 = ConcatIdx[off0 + (segInE * 2 + 0)];
            const uint iBot0 = ConcatIdx[off0 + (segInE * 2 + 1)];
            const uint iTop1 = ConcatIdx[off0 + ((segInE + 1) * 2 + 0)];
            const uint iBot1 = ConcatIdx[off0 + ((segInE + 1) * 2 + 1)];

            ParticleOutput Top0 = InPoints[iTop0];
            ParticleOutput Bot0 = InPoints[iBot0];
            ParticleOutput Top1 = InPoints[iTop1];
            ParticleOutput Bot1 = InPoints[iBot1];

            // ── 센터/폭/방향 기초값 ──────────────────────────────
            float3 C0 = 0.5f * (Top0.Position.xyz + Bot0.Position.xyz);
            float3 C1 = 0.5f * (Top1.Position.xyz + Bot1.Position.xyz);

            float W0 = 0.5f * length(Top0.Position.xyz - Bot0.Position.xyz);
            float W1 = 0.5f * length(Top1.Position.xyz - Bot1.Position.xyz);

            // S: Paddings에 저장된 폭 방향 (노말/사이드) — 플립가드
            float3 S0 = normalize(Top0.Paddings);
            float3 S1 = normalize(Top1.Paddings);
            if (dot(S0, S1) < 0.0f)
                S1 = -S1;

            // ── Hermite용 이웃(클램프) ──────────────────────────
            const uint a0 = segInE;
            const uint a1 = segInE + 1;
            const uint am1 = (a0 == 0) ? 0 : (a0 - 1);
            const uint a2 = (a1 + 1 >= anchors) ? (anchors - 1) : (a1 + 1);

            const uint iT_m1 = ConcatIdx[off0 + (am1 * 2 + 0)];
            const uint iB_m1 = ConcatIdx[off0 + (am1 * 2 + 1)];
            const uint iT_2 = ConcatIdx[off0 + (a2 * 2 + 0)];
            const uint iB_2 = ConcatIdx[off0 + (a2 * 2 + 1)];

            float3 C_m1 = 0.5f * (InPoints[iT_m1].Position.xyz + InPoints[iB_m1].Position.xyz);
            float3 C_2 = 0.5f * (InPoints[iT_2].Position.xyz + InPoints[iB_2].Position.xyz);

            // 출력 시작 인덱스
            const uint F = bit32_4_tessellationParams.TessFactor;
            const uint outBase = EmitStarts[e] + segInE * (F * 2);

            // tension 파라미터 (0.0~0.25 권장)
            const float tension = 0.15f;

            for (uint k = 0; k < F; ++k)
            {
                const float t = (F <= 1) ? 0.0f : (float) k / (float) (F - 1);

                // 1) 센터라인: Hermite(카디널) 보간
                float3 C = HermiteCardinal(C_m1, C0, C1, C_2, t, tension);

                // 2) 폭: 선형 보간 + 최소값 클램프(폭 0 → NaN 방지)
                float W = max(lerp(W0, W1, t), 1e-5f);

                // 3) 방향: S 보간(+정규화). 길이가 거의 0이면 S0 사용
                float3 S = lerp(S0, S1, t);
                float l2 = dot(S, S);
                S = (l2 < 1e-8f) ? S0 : normalize(S);

                // --- Top ---
                {
                    ParticleOutput O = Top0; // 메타 seed
                    O.Position.xyz = C + S * W; // 최종 위치
                    O.Position.w = Top0.Position.w;

                    O.Color = lerp(Top0.Color, Top1.Color, t);
                    O.FrameInfo = lerp(Top0.FrameInfo, Top1.FrameInfo, t);

                    float3 pad = lerp(Top0.Paddings, Top1.Paddings, t);
                    float lp2 = dot(pad, pad);
                    O.Paddings = (lp2 < 1e-8f) ? Top0.Paddings : normalize(pad);

                    OutPoints[outBase + (k * 2 + 0)] = O;
                }

                // --- Bottom ---
                {
                    ParticleOutput O = Bot0;
                    O.Position.xyz = C - S * W;
                    O.Position.w = Bot0.Position.w;

                    O.Color = lerp(Bot0.Color, Bot1.Color, t);
                    O.FrameInfo = lerp(Bot0.FrameInfo, Bot1.FrameInfo, t);

                    float3 pad = lerp(Bot0.Paddings, Bot1.Paddings, t);
                    float lp2 = dot(pad, pad);
                    O.Paddings = (lp2 < 1e-8f) ? Bot0.Paddings : normalize(pad);

                    OutPoints[outBase + (k * 2 + 1)] = O;
                }
            }
            break;
        }

        accumSeg += segCountE;
        ++e;
    }
}
