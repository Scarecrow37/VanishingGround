//#define RADIX_SIZE         16
//#define MAX_THREAD_GROUPS 256

//cbuffer RadixSortConstants : register(b0)
//{
//    uint g_numParticles; // 파티클 수[1]
//    uint g_currentBit; // 처리할 비트 인덱스[1]
//    uint g_numThreadGroups; // 실제 스레드 그룹 수[1]
//    uint g_pad;
//};

//RWStructuredBuffer<uint> g_histogram : register(u2); // 히스토그램 버퍼[2]
//RWStructuredBuffer<uint> g_prefixSum : register(u3); // prefix sum 결과 버퍼[2]

//groupshared uint g_sharedData[RADIX_SIZE]; // bucket별 공유 메모리[2]

//[numthreads(RADIX_SIZE, 1, 1)]
//void cs_main(uint3 groupId : SV_GroupID, // 스레드 그룹 식별자[3]
//             uint3 threadId3D : SV_GroupThreadID)  // 그룹 내 스레드 로컬 인덱스[3]
//{
//    uint bucket = threadId3D.x;

//    // 모든 스레드는 흐름을 동일하게 유지
//    bool isActive = (bucket < RADIX_SIZE && g_numThreadGroups != 0);

//    uint value = 0;
//    if (isActive)
//        value = g_histogram[groupId.x * RADIX_SIZE + bucket];

//    g_sharedData[bucket] = value;

//    GroupMemoryBarrierWithGroupSync(); // ✅ 모든 스레드가 호출함

//    uint sum = 0;
//    if (isActive)
//    {
//        for (uint i = 0; i < RADIX_SIZE; ++i)
//        {
//            sum += g_sharedData[i];
//            if (i == bucket)
//                break;
//        }
//    }

//    GroupMemoryBarrierWithGroupSync();

//    if (isActive)
//    {
//        g_prefixSum[bucket] = sum;
//        g_histogram[groupId.x * RADIX_SIZE + bucket] = g_sharedData[bucket];
//    }
//}


// cs_radix_prefixsum.hlsl
#define RADIX_SIZE         16
#define MAX_THREAD_GROUPS 256

cbuffer RadixSortConstants : register(b0)
{
    uint g_numParticles; // 파티클 수
    uint g_currentBit; // 처리할 비트 인덱스
    uint g_numThreadGroups; // 실제 스레드 그룹 수
    uint g_pad;
};

RWStructuredBuffer<uint> g_histogram : register(u2);
RWStructuredBuffer<uint> g_prefixSum : register(u3);

groupshared uint g_sharedData[RADIX_SIZE];

[numthreads(RADIX_SIZE, 1, 1)]
void cs_main(uint3 groupId : SV_GroupID,
             uint3 threadId3D : SV_GroupThreadID)
{
    uint bucket = threadId3D.x;
    // 모든 스레드 동일 흐름 유지
    bool isActive = (bucket < RADIX_SIZE && g_numThreadGroups > 0);

    // 1) 읽어서 공유 메모리에 쓰기
    uint value = 0;
    if (isActive)
        value = g_histogram[groupId.x * RADIX_SIZE + bucket];
    g_sharedData[bucket] = value;

    GroupMemoryBarrierWithGroupSync(); // ← 필수

    // 2) prefix-sum(스캔)
    uint sum = 0;
    if (isActive)
    {
        for (uint i = 0; i < RADIX_SIZE; ++i)
        {
            sum += g_sharedData[i];
            if (i == bucket)
                break;
        }
    }

    GroupMemoryBarrierWithGroupSync(); // ← 필수

    // 3) 결과 저장
    if (isActive)
    {
        g_prefixSum[groupId.x * RADIX_SIZE + bucket] = sum;
        // (원본 히스토그램 유지용이라면 그대로 덮어써도 무방)
        g_histogram[groupId.x * RADIX_SIZE + bucket] = g_sharedData[bucket];
    }
}
