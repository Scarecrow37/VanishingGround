#include "CommonData.hlsli"

Texture2D<float4> equirectangularMap;
RWTexture2DArray<float4> cubeMap;

struct CubeMapInfo
{
    uint resolution;
};

ConstantBuffer<CubeMapInfo> bit32_1_cubeMapInfo;

[numthreads(16, 16, 1)]
void cs_main(uint3 DTid : SV_DispatchThreadID)
{
    uint x = DTid.x;
    uint y = DTid.y;

    if (x >= bit32_1_cubeMapInfo.resolution || y >= bit32_1_cubeMapInfo.resolution)
        return;
    
    float u = (2.f * (float(x) + 0.5f) / bit32_1_cubeMapInfo.resolution) - 1.f;
    float v = (2.f * (float(y) + 0.5f) / bit32_1_cubeMapInfo.resolution) - 1.f;

    float3 dir;
    switch (DTid.z)
    {
        case 0:
            dir = normalize(float3(1.0f, v, -u));
            break; // +X
        case 1:
            dir = normalize(float3(-1.0f, v, u));
            break; // -X
        case 2:
            dir = normalize(float3(u, -1.0f, v));
            break; // +Y
        case 3:
            dir = normalize(float3(u, 1.0f, -v));
            break; // -Y
        case 4:
            dir = normalize(float3(u, v, 1.0f));
            break; // +Z
        case 5:
            dir = normalize(float3(-u, v, -1.0f));
            break; // -Z
    }
    
    // 방향 벡터 → equirectangular UV 변환
    float2 uv;
    uv.x = atan2(dir.z, dir.x) / (2.0f * 3.14159265f) + 0.5f;
    uv.y = asin(clamp(dir.y, -1.0f, 1.0f)) / 3.14159265f + 0.5f;

    float4 color = equirectangularMap.SampleLevel(samLinear_clamp, uv, 0.0f);

    cubeMap[DTid] = color;
}