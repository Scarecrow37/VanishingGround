#define RADIX_SIZE 16
#define MAX_THREAD_GROUPS 512

cbuffer RadixSortConstants : register(b0)
{
    uint g_numParticles;
    uint g_currentBit;
    uint g_numThreadGroups;
    uint g_pad;
};

RWStructuredBuffer<uint> g_histogram : register(u2);
RWStructuredBuffer<uint> g_prefixSum : register(u3);

groupshared uint g_sharedData[RADIX_SIZE * MAX_THREAD_GROUPS];

[numthreads(RADIX_SIZE, 1, 1)]
void cs_main(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    // 입력 파라미터 검증
    if (groupIndex >= RADIX_SIZE)
        return;
        
    if (g_numThreadGroups == 0 || g_numThreadGroups > MAX_THREAD_GROUPS)
        return;
    
    // 안전한 히스토그램 합산
    uint sum = 0;
    uint validThreadGroups = min(g_numThreadGroups, MAX_THREAD_GROUPS);
    
    for (uint i = 0; i < validThreadGroups; i++)
    {
        uint histogramIndex = i * RADIX_SIZE + groupIndex;
        
        // 공유 메모리 범위 검증
        if (i < MAX_THREAD_GROUPS)
        {
            uint value = g_histogram[histogramIndex];
            g_sharedData[i] = value;
            
            // 오버플로우 검증
            if (sum <= 0xFFFFFFFF - value)
            {
                sum += value;
            }
            else
            {
                sum = 0xFFFFFFFF; // 오버플로우 시 최대값으로 설정
                break;
            }
        }
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    // 안전한 접두사 합 계산 (Blelloch scan)
    uint prefixValue = 0;
    for (uint j = 0; j < validThreadGroups; j++)
    {
        if (j < MAX_THREAD_GROUPS)
        {
            uint temp = g_sharedData[j];
            g_sharedData[j] = prefixValue;
            
            // 오버플로우 방지
            if (prefixValue <= 0xFFFFFFFF - temp)
            {
                prefixValue += temp;
            }
            else
            {
                prefixValue = 0xFFFFFFFF;
                break;
            }
        }
    }
    
    GroupMemoryBarrierWithGroupSync();
    
    // 안전한 결과 저장
    for (uint k = 0; k < validThreadGroups; k++)
    {
        if (k < MAX_THREAD_GROUPS)
        {
            uint histogramIndex = k * RADIX_SIZE + groupIndex;
            
            // 범위 검증 후 저장
            if (histogramIndex < g_numThreadGroups * RADIX_SIZE)
            {
                uint temp = g_sharedData[k];
                g_histogram[histogramIndex] = temp;
            }
        }
    }
    
    // 글로벌 접두사 합 저장 (범위 검증)
    if (groupIndex < RADIX_SIZE)
    {
        g_prefixSum[groupIndex] = prefixValue;
    }
}
