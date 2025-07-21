#include "CommonData.hlsli"

// 입력: 1단계에서 생성된 Environment Cubemap
TextureCube environmentMap;

// 출력: Irradiance 큐브맵 (UAV)
RWTexture2DArray<float4> irradianceMap;

static const uint NUM_SAMPLES = 1024u; // 샘플링 횟수 (품질과 성능에 영향)

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

// 코사인 가중 중요도 샘플링을 위한 반구 샘플 벡터 생성
float3 ImportanceSampleGGX(float2 Xi, float3 N, float roughness)
{
    // 이 함수는 Specular에 사용되지만, Diffuse에서는 간단한 코사인 샘플링을 사용합니다.
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt(1.0 - Xi.y);
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

[numthreads(8, 8, 1)]
void cs_main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint width, height, depth;
    irradianceMap.GetDimensions(width, height, depth);

    if (dispatchThreadID.x >= width || dispatchThreadID.y >= height)
    {
        return;
    }

    // 현재 픽셀의 Normal 벡터 계산
    float3 N = GetDirectionForCubeFace(dispatchThreadID.xy, dispatchThreadID.z, width);

    float3 irradiance = float3(0.0, 0.0, 0.0);
    
    // Normal 벡터 주변의 반구(hemisphere)를 몬테카를로 적분
    for (uint i = 0; i < NUM_SAMPLES; ++i)
    {
        float2 Xi = Hammersley(i, NUM_SAMPLES);
        float3 H = ImportanceSampleGGX(Xi, N, 1.0); // Diffuse는 roughness 1.0과 유사
        
        // 샘플 방향으로 큐브맵 샘플링
        irradiance += environmentMap.SampleLevel(samLinear_clamp, H, 0).rgb;
    }
    irradiance = irradiance / float(NUM_SAMPLES);

    irradianceMap[dispatchThreadID] = float4(irradiance, 1.0);
}