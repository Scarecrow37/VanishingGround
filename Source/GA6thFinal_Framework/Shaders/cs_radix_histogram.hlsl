//// cs_radix_histogram.hlsl
//// 4-bit 라딕스에 대한 히스토그램 계산

//#define RADIX_BITS 4
//#define RADIX_SIZE (1 << RADIX_BITS) // 16
//#define THREADS_PER_GROUP 256
//#define MAX_SAFE_PARTICLES 1048576  // 1M 파티클 제한

//cbuffer RadixSortConstants : register(b0)
//{
//    uint g_numParticles;
//    uint g_currentBit;
//    uint g_numThreadGroups;
//    uint g_pad;
//};

//StructuredBuffer<float> g_sortKeys : register(t0);
//RWStructuredBuffer<uint> g_histogram : register(u2);

//groupshared uint g_localHistogram[RADIX_SIZE];

//[numthreads(THREADS_PER_GROUP, 1, 1)]
//void cs_main(uint3 groupId : SV_GroupID, uint groupIndex : SV_GroupIndex)
//{
//    // 입력 파라미터 안전성 검증
//    bool validInput = (g_numParticles > 0 && g_numParticles <= MAX_SAFE_PARTICLES &&
//                      g_numThreadGroups > 0 && g_numThreadGroups <= 65535);
    
//    // 로컬 히스토그램 초기화 (모든 스레드 참여)
//    if (groupIndex < RADIX_SIZE)
//    {
//        g_localHistogram[groupIndex] = 0;
//    }
    
//    // 첫 번째 동기화 - 모든 스레드가 반드시 도달
//    GroupMemoryBarrierWithGroupSync();
    
//    // 안전한 범위 계산
//    uint totalThreads = validInput ? (g_numThreadGroups * THREADS_PER_GROUP) : 1;
//    uint safeParticleCount = validInput ? min(g_numParticles, MAX_SAFE_PARTICLES) : 0;
    
//    uint elementsPerThread = validInput ?
//        ((safeParticleCount + totalThreads - 1) / totalThreads) : 0;
    
//    uint threadGlobalIndex = groupId.x * THREADS_PER_GROUP + groupIndex;
//    uint startIndex = threadGlobalIndex * elementsPerThread;
//    uint endIndex = min(startIndex + elementsPerThread, safeParticleCount);
    
//    // 범위 검증 마스킹 (early return 사용하지 않음)
//    bool isValidRange = validInput && (startIndex < safeParticleCount);
    
//    // 히스토그램 계산 (마스킹 사용)
//    for (uint i = startIndex; i < endIndex; i++)
//    {
//        // 이중 안전 검사 (조건문이 아닌 마스킹)
//        bool validIndex = isValidRange && (i < safeParticleCount) && (i < MAX_SAFE_PARTICLES);
        
//        // 안전한 키 읽기 (유효하지 않으면 0 사용)
//        uint key = validIndex ? asuint(g_sortKeys[i]) : 0;
        
//        // 비트 시프트 범위 검증 (0-28 비트만 유효)
//        bool validBitShift = validInput && (g_currentBit <= 28);
//        uint digit = (validIndex && validBitShift) ?
//            ((key >> g_currentBit) & (RADIX_SIZE - 1)) : 0;
        
//        // 디지트 범위 검증 후 히스토그램 업데이트
//        bool validDigit = (digit < RADIX_SIZE);
//        uint incrementValue = (validIndex && validBitShift && validDigit) ? 1 : 0;
        
//        if (incrementValue > 0)
//        {
//            InterlockedAdd(g_localHistogram[digit], incrementValue);
//        }
//    }
    
//    // 두 번째 동기화 - 모든 스레드가 반드시 도달
//    GroupMemoryBarrierWithGroupSync();
    
//    // 글로벌 히스토그램에 결과 병합 (안전한 범위 검증)
//    if (groupIndex < RADIX_SIZE)
//    {
//        uint globalIndex = groupId.x * RADIX_SIZE + groupIndex;
//        bool validGlobalIndex = validInput &&
//            (globalIndex < (g_numThreadGroups * RADIX_SIZE)) &&
//            (globalIndex < 1048576); // 히스토그램 버퍼 최대 크기 제한
        
//        if (validGlobalIndex)
//        {
//            g_histogram[globalIndex] = g_localHistogram[groupIndex];
//        }
//    }
//}

// cs_radix_histogram.hlsl
#define RADIX_BITS 4
#define RADIX_SIZE (1 << RADIX_BITS) // 16
#define THREADS_PER_GROUP 256
#define MAX_SAFE_PARTICLES 1048576  // 1M 파티클 제한

cbuffer RadixSortConstants : register(b0)
{
    uint g_numParticles;
    uint g_currentBit;
    uint g_numThreadGroups;
    uint g_pad;
};

StructuredBuffer<float> g_sortKeys : register(t0);
RWStructuredBuffer<uint> g_histogram : register(u2);

groupshared uint g_localHistogram[RADIX_SIZE];

[numthreads(THREADS_PER_GROUP, 1, 1)]
void cs_main(uint3 groupId : SV_GroupID,
             uint groupIndex : SV_GroupIndex)
{
    // 1) 입력 유효성 마스크
    bool validInput = (g_numParticles > 0 && g_numParticles <= MAX_SAFE_PARTICLES &&
                       g_numThreadGroups > 0 && g_numThreadGroups <= 65535 &&
                       g_currentBit <= 28);

    // 2) 로컬 히스토그램 초기화 (모든 스레드 동일하게 접근)
    if (groupIndex < RADIX_SIZE)
        g_localHistogram[groupIndex] = 0;

    GroupMemoryBarrierWithGroupSync(); // ← 모든 스레드가 반드시 호출

    // 3) 각 스레드가 처리할 범위 계산
    uint totalThreads = g_numThreadGroups * THREADS_PER_GROUP;
    // validInput==false 시 elementsPerThread=0 이 되어 아무 것도 하지 않음
    uint elementsPerThread = validInput
        ? ((g_numParticles + totalThreads - 1) / totalThreads)
        : 0;

    uint threadGlobalIndex = groupId.x * THREADS_PER_GROUP + groupIndex;
    uint startIndex = threadGlobalIndex * elementsPerThread;
    uint endIndex = min(startIndex + elementsPerThread, g_numParticles);

    // 4) 히스토그램 집계 (조건은 모두 마스크로 처리)
    for (uint i = startIndex; i < endIndex; ++i)
    {
        uint key = asuint(g_sortKeys[i]);
        uint digit = (key >> g_currentBit) & (RADIX_SIZE - 1);

        // validInput==false 이거나 i>=g_numParticles 이면 incrementValue=0
        uint incrementValue = (validInput && digit < RADIX_SIZE) ? 1u : 0u;
        if (incrementValue > 0)
            InterlockedAdd(g_localHistogram[digit], incrementValue);
    }

    GroupMemoryBarrierWithGroupSync(); // ← 모든 스레드가 반드시 호출

    // 5) 글로벌 히스토그램으로 복사
    if (groupIndex < RADIX_SIZE)
    {
        uint globalIndex = groupId.x * RADIX_SIZE + groupIndex;
        bool ok = validInput && (globalIndex < g_numThreadGroups * RADIX_SIZE);
        if (ok)
            g_histogram[globalIndex] = g_localHistogram[groupIndex];
    }
}


