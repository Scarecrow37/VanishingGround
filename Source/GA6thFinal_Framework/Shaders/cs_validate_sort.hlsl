// cs_prefix_sum.hlsl
// 히스토그램의 접두사 합 계산 (Parallel Prefix Sum)

#define RADIX_SIZE 16
#define MAX_THREAD_GROUPS 512

// 통합 루트 시그니처 파라미터 매핑
cbuffer RadixSortConstants : register(b0) // [0] CBV
{
    uint g_numParticles;
    uint g_currentBit;
    uint g_numThreadGroups;
    uint g_pad;
};

// t0, t1은 이 셰이더에서 사용하지 않음
// u0, u1은 이 셰이더에서 사용하지 않음
RWStructuredBuffer<uint> g_histogram : register(u2); // [5] UAV - 히스토그램 버퍼 (읽기/쓰기)
RWStructuredBuffer<uint> g_prefixSum : register(u3); // [6] UAV - 접두사 합 버퍼
// u4는 이 셰이더에서 사용하지 않음

groupshared uint g_sharedData[RADIX_SIZE * MAX_THREAD_GROUPS];

[numthreads(RADIX_SIZE, 1, 1)]
void cs_main(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    // 각 라딕스 값에 대해 모든 스레드 그룹의 히스토그램을 합산
    uint sum = 0;
    for (uint i = 0; i < g_numThreadGroups; i++)
    {
        uint histogramIndex = i * RADIX_SIZE + groupIndex;
        g_sharedData[i] = g_histogram[histogramIndex];
        sum += g_sharedData[i];
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    // 접두사 합 계산 (Blelloch scan 알고리즘 사용)
    uint prefixValue = 0;
    for (uint j = 0; j < g_numThreadGroups; j++)
    {
        uint temp = g_sharedData[j];
        g_sharedData[j] = prefixValue;
        prefixValue += temp;
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    // 결과를 다시 히스토그램 버퍼에 저장 (오프셋으로 사용)
    for (uint k = 0; k < g_numThreadGroups; k++)
    {
        uint histogramIndex = k * RADIX_SIZE + groupIndex;
        g_histogram[histogramIndex] = g_sharedData[k];
    }
    
    // 글로벌 접두사 합도 저장
    g_prefixSum[groupIndex] = prefixValue;
}
