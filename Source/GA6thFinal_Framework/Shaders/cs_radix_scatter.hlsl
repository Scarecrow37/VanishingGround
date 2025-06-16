#define RADIX_BITS 4
#define RADIX_SIZE (1 << RADIX_BITS) // 16
#define THREADS_PER_GROUP 512
#define MAX_SAFE_PARTICLES 1048576

cbuffer RadixSortConstants : register(b0)
{
    uint g_numParticles;
    uint g_currentBit;
    uint g_numThreadGroups;
    uint g_pad;
};

StructuredBuffer<float> g_inputKeys : register(t0);
StructuredBuffer<uint> g_inputValues : register(t1);
RWStructuredBuffer<float> g_outputKeys : register(u0);
RWStructuredBuffer<uint> g_outputValues : register(u1);
RWStructuredBuffer<uint> g_histogram : register(u2);
RWStructuredBuffer<uint> g_prefixSum : register(u3);

groupshared uint g_localOffset[RADIX_SIZE];

// 안전한 키 변환 함수 (히스토그램과 동일)
uint TransformKeyForSorting(float key)
{
    uint bits = asuint(key);
    uint exponent = (bits >> 23) & 0xFF;
    
    // 특수 값 처리
    if (exponent == 0xFF)
    {
        return 0xFFFFFFFF;
    }
    
    return bits;
}

[numthreads(THREADS_PER_GROUP, 1, 1)]
void cs_main(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex)
{
    // 다중 범위 검증
    if (g_numParticles == 0 || g_numParticles > MAX_SAFE_PARTICLES)
        return;
        
    if (g_numThreadGroups == 0 || groupId.x >= g_numThreadGroups)
        return;
    
    // 현재 그룹의 히스토그램 오프셋을 공유 메모리로 로드 (안전한 범위 검증)
    if (groupIndex < RADIX_SIZE)
    {
        uint histogramIndex = groupId.x * RADIX_SIZE + groupIndex;
        
        // 범위 검증 후 로드
        if (histogramIndex < g_numThreadGroups * RADIX_SIZE)
        {
            g_localOffset[groupIndex] = g_histogram[histogramIndex];
        }
        else
        {
            g_localOffset[groupIndex] = 0;
        }
    }
    GroupMemoryBarrierWithGroupSync();
    
    // 안전한 요소 계산
    uint totalThreads = g_numThreadGroups * THREADS_PER_GROUP;
    if (totalThreads == 0)
        return;
        
    uint elementsPerThread = (g_numParticles + totalThreads - 1) / totalThreads;
    
    uint threadGlobalIndex = groupId.x * THREADS_PER_GROUP + groupIndex;
    uint startIndex = threadGlobalIndex * elementsPerThread;
    uint endIndex = min(startIndex + elementsPerThread, g_numParticles);
    
    // 범위 검증
    if (startIndex >= g_numParticles || startIndex >= MAX_SAFE_PARTICLES)
        return;
    
    // 각 원소를 올바른 위치로 스캐터
    for (uint i = startIndex; i < endIndex && i < g_numParticles; i++)
    {
        // 버퍼 오버플로우 방지
        if (i >= MAX_SAFE_PARTICLES)
            break;
        
        // 안전한 데이터 읽기
        float rawKey = g_inputKeys[i];
        uint value = g_inputValues[i];
        
        // 안전한 키 변환 및 자릿수 추출
        uint key = TransformKeyForSorting(rawKey);
        
        // 비트 시프트 범위 검증
        if (g_currentBit > 28)
            continue;
            
        uint digit = (key >> g_currentBit) & (RADIX_SIZE - 1);
        
        // 디지트 값 범위 검증
        if (digit >= RADIX_SIZE)
            digit = RADIX_SIZE - 1; // 최대값으로 클램핑
        
        // 원자적 증가를 통해 출력 위치 결정
        uint outputIndex;
        InterlockedAdd(g_localOffset[digit], 1, outputIndex);
        
        // 최종 인덱스 범위 검증
        if (outputIndex < g_numParticles && outputIndex < MAX_SAFE_PARTICLES)
        {
            // 새로운 위치에 키-값 쌍 저장
            g_outputKeys[outputIndex] = rawKey;
            g_outputValues[outputIndex] = value;
        }
    }
}
