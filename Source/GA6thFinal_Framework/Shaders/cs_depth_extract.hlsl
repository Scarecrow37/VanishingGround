#include "Compute.hlsli"

cbuffer RadixSortConstants : register(b0)
{
    uint g_numParticles;
    uint g_currentBit;
    uint g_numThreadGroups;
    uint g_pad;
};

StructuredBuffer<ParticleOutput> g_inputParticles : register(t0);
RWStructuredBuffer<float> g_sortKeys : register(u0);
RWStructuredBuffer<uint> g_sortValues : register(u1);

// IEEE 754 특수 값 검증 함수
bool IsValidFloat(float value)
{
    uint bits = asuint(value);
    uint exponent = (bits >> 23) & 0xFF;
    uint mantissa = bits & 0x7FFFFF;
    
    // NaN 검사: 지수가 모두 1이고 가수가 0이 아님
    if (exponent == 0xFF && mantissa != 0)
        return false;
        
    // Infinity 검사: 지수가 모두 1이고 가수가 0
    if (exponent == 0xFF && mantissa == 0)
        return false;
        
    return true;
}

// 안전한 IEEE 754 변환 함수
uint TransformFloatForSorting(float value)
{
    // 특수 값 처리
    if (!IsValidFloat(value))
    {
        // NaN이나 Infinity는 최대값으로 처리 (가장 뒤로 정렬)
        return 0xFFFFFFFF;
    }
    
    uint bits = asuint(value);
    
    // 음수 처리를 위한 IEEE 754 표준 변환
    if ((bits & 0x80000000) != 0)
        return ~bits;
    else
        return bits | 0x80000000;
}

// Depth 값 양자화로 플리커링 방지
float QuantizeDepth(float depth)
{
    const float DEPTH_QUANTUM = 0.0001f; // 더 세밀한 양자화
    return floor(depth / DEPTH_QUANTUM) * DEPTH_QUANTUM;
}

[numthreads(256, 1, 1)]
void cs_main(uint3 id : SV_DispatchThreadID)
{
    // 스레드 인덱스 범위 검증
    if (id.x >= g_numParticles)
        return;
    
    // 안전한 메모리 액세스를 위한 추가 검증
    if (id.x >= 1000000) // 최대 파티클 수 제한 (1M)
        return;
    
    // 파티클 데이터 읽기
    ParticleOutput particle = g_inputParticles[id.x];
    
    // Depth 값 추출 및 검증
    float rawDepth = particle.position.z;
    
    
    // Back-to-front 정렬을 위한 음수 처리
    float depth = -rawDepth;
    
    // Depth 양자화로 플리커링 방지
    depth = QuantizeDepth(depth);
    
    // 안전한 변환
    uint depthAsUint = TransformFloatForSorting(depth);
    
    // 출력 버퍼 범위 검증 후 쓰기
    if (id.x < g_numParticles)
    {
        float temp = asfloat(depthAsUint);
        g_sortKeys[id.x] = temp;
        g_sortValues[id.x] = id.x;
    }
}
