#include "CommonData.hlsli"

// 출력: 2D BRDF LUT (UAV)
RWTexture2D<float2> brdfLUT;

// 상수
static const uint NUM_SAMPLES = 1024u;

struct BRDFConstants
{
    uint2 offset; // 현재 처리할 픽셀의 오프셋
};

ConstantBuffer<BRDFConstants> bit32_2_brdfConstants;

// Low-discrepancy 시퀀스 (Hammersley)
float2 Hammersley(uint i, uint N)
{
    // i의 비트를 역순으로 뒤집는 연산
    uint bits = (i << 16u) | (i >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

    // ULL 접미사 대신 float 리터럴을 사용하도록 수정
    // 0x100000000ULL -> 4294967296.0f
    return float2(float(i) / float(N), float(bits) / 4294967296.0f);
}

// GGX 중요도 샘플링
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

// Schlick-GGX 기하 감쇠 함수
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float a = roughness;
    float k = (a * a) / 2.0;
    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    return nom / denom;
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

[numthreads(8, 8, 1)]
void cs_main(uint2 dispatchThreadID : SV_DispatchThreadID)
{
    // 스레드 그룹 내 로컬 ID (0~7)
    uint2 localID = dispatchThreadID.xy;

    // 전체 픽셀 좌표 계산
    uint2 absolutePixelCoord = bit32_2_brdfConstants.offset + localID;
    
    uint width, height;
    brdfLUT.GetDimensions(width, height);

    if (absolutePixelCoord.x >= width || absolutePixelCoord.y >= height)
    {
        return;
    }

    // 현재 픽셀의 UV를 NdotV와 roughness 값으로 변환
    float2 uv = ((float2)absolutePixelCoord.xy + 0.5f) / float2(width, height);
    float NdotV = uv.x;
    float roughness = uv.y;

    float3 V;
    V.x = sqrt(1.0 - NdotV * NdotV);
    V.y = 0;
    V.z = NdotV;

    float3 N = float3(0.0, 0.0, 1.0);

    float A = 0.0;
    float B = 0.0;

    for (uint i = 0; i < NUM_SAMPLES; ++i)
    {
        float2 Xi = Hammersley(i, NUM_SAMPLES);
        float3 H = ImportanceSampleGGX(Xi, N, roughness);
        float3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);
        float VdotH = max(dot(V, H), 0.0);

        if (NdotL > 0.0)
        {
            float G = GeometrySmith(N, V, L, roughness);
            float G_Vis = (G * VdotH) / (NdotH * NdotV);
            float Fc = pow(1.0 - VdotH, 5.0);
            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    
    A /= float(NUM_SAMPLES);
    B /= float(NUM_SAMPLES);

    brdfLUT[absolutePixelCoord] = float2(A, B);
}