#include "Compute.hlsli"

#define MAX_SAFE_PARTICLES 1048576

cbuffer RadixSortConstants : register(b0)
{
    uint g_numParticles;
    uint g_currentBit;
    uint g_numThreadGroups;
    uint g_pad;
};

StructuredBuffer<uint> g_sortedIndices : register(t0);
StructuredBuffer<ParticleOutput> g_inputParticles : register(t1);
RWStructuredBuffer<ParticleOutput> g_outputParticles : register(u4);

// 파티클 데이터 유효성 검증 함수
bool IsValidParticleOutput(ParticleOutput particle)
{
    // 매트릭스 값들의 유효성 검사
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            float value = particle.FinalMatrix[i][j];
            uint bits = asuint(value);
            uint exponent = (bits >> 23) & 0xFF;
            uint mantissa = bits & 0x7FFFFF;
            
            // NaN 또는 Infinity 검사
            if (exponent == 0xFF)
                return false;
        }
    }
    
    return true;
}

[numthreads(256, 1, 1)]
void cs_main(uint3 id : SV_DispatchThreadID)
{
    // 다중 범위 검증
    if (id.x >= g_numParticles || id.x >= MAX_SAFE_PARTICLES)
        return;
        
    if (g_numParticles == 0)
        return;
    
    // 안전한 인덱스 읽기
    uint sortedIndex = g_sortedIndices[id.x];
    
    // 정렬된 인덱스 범위 검증
    if (sortedIndex >= g_numParticles || sortedIndex >= MAX_SAFE_PARTICLES)
        return;
    
    // 안전한 파티클 데이터 읽기
    ParticleOutput particle = g_inputParticles[sortedIndex];
    
    // 파티클 데이터 유효성 검증
    if (!IsValidParticleOutput(particle))
    {
        // 유효하지 않은 데이터의 경우 기본값으로 초기화
        particle = (ParticleOutput) 0;
        particle.FinalMatrix = float4x4(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 1000, // 매우 먼 거리로 설정
            0, 0, 0, 1
        );
    }
    
    // 안전한 출력 (범위 재검증)
    if (id.x < g_numParticles && id.x < MAX_SAFE_PARTICLES)
    {
        g_outputParticles[id.x] = particle;
    }
}
