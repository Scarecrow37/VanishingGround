#include "CommonData.hlsli"

// 상수 버퍼: 현재 처리할 밉맵 레벨의 roughness 값을 받음
struct PreFilterConstants
{
    float Roughness; // 현재 밉맵 레벨의 roughness 값
    float Resolution; // 원본 큐브맵의 해상도
};

ConstantBuffer<PreFilterConstants> bit32_2_preFilter;

// 입력: 1단계에서 생성된 Environment Cubemap
TextureCube environmentMap;

// 출력: Specular 큐브맵의 특정 밉 레벨 (UAV)
// C++ 코드에서 각 밉 레벨에 맞는 UAV를 생성하여 바인딩해야 함
RWTexture2DArray<float4> prefilteredMap;

// 상수
static const uint NUM_SAMPLES = 1024u;

// 1단계와 동일한 함수
float3 GetDirectionForCubeFace(uint2 pixelCoords, uint faceIndex, float cubeSize)
{
    float2 uv = (float2(pixelCoords) + 0.5f) / cubeSize * 2.0f - 1.0f;
    uv.y = -uv.y;
    float3 dir = float3(0, 0, 0);
    switch (faceIndex)
    {
        case 0:
            dir = float3(1.0, uv.y, -uv.x);
            break; // +X
        case 1:
            dir = float3(-1.0, uv.y, uv.x);
            break; // -X
        case 2:
            dir = float3(uv.x, 1.0, -uv.y);
            break; // +Y
        case 3:
            dir = float3(uv.x, -1.0, uv.y);
            break; // -Y
        case 4:
            dir = float3(uv.x, uv.y, 1.0);
            break; // +Z
        case 5:
            dir = float3(-uv.x, uv.y, -1.0);
            break; // -Z
    }
    return normalize(dir);
}

// Low-discrepancy 시퀀스 (Hammersley)
float2 Hammersley(uint i, uint N)
{
    uint bits = (i << 16u) | (i >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float2(float(i) / float(N), float(bits) / 0x100000000UL);
}

// GGX 중요도 샘플링을 위한 반구 샘플 벡터 생성
float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
    float a = roughness * roughness;
    
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    float3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;

    float3 up = abs(N.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    float3 tangent = normalize(cross(up, N));
    float3 bitangent = cross(N, tangent);

    float3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}

[numthreads(16, 16, 1)]
void cs_main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint width, height, depth;
    prefilteredMap.GetDimensions(width, height, depth);

    if (dispatchThreadID.x >= width || dispatchThreadID.y >= height)
    {
        return;
    }

    float3 N = GetDirectionForCubeFace(dispatchThreadID.xy, dispatchThreadID.z, width);
    float3 V = N;

    float3 prefilteredColor = float3(0.0, 0.0, 0.0);
    float totalWeight = 0.0;

    for (uint i = 0; i < NUM_SAMPLES; ++i)
    {
        float2 Xi = Hammersley(i, NUM_SAMPLES);
        float3 H = ImportanceSampleGGX(Xi, N, bit32_2_preFilter.Roughness);
        float3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        if (NdotL > 0.0)
        {
            // --- 여기가 수정된 부분 ---
            // Solid Angle of the sample
            float solidAngle = 4.0 * PI / (6.0 * bit32_2_preFilter.Resolution * bit32_2_preFilter.Resolution);
            // Solid angle of the mip level
            float mipSolidAngle = 4.0 * PI / (float(NUM_SAMPLES));
            // Calculate the lod level to sample
            float lod = 0.5 * log2(mipSolidAngle / solidAngle);
            // --- 수정 끝 ---
            
            prefilteredColor += environmentMap.SampleLevel(samLinear_clamp, L, lod).rgb * NdotL;
            totalWeight += NdotL;
        }
    }

    prefilteredColor = (totalWeight > 0.0) ? (prefilteredColor / totalWeight) : float3(0.0, 0.0, 0.0);

    prefilteredMap[dispatchThreadID] = float4(prefilteredColor, 1.0);
}