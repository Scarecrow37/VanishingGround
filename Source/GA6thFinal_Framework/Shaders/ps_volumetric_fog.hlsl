#include "CommonData.hlsli"
#include "Function.hlsli"

Texture2D<float4> screenMap;
Texture2D<float> depthMap;
Texture3D<float4> fogGridTexture;

cbuffer VolumetricFogCompositeData
{
    matrix ViewProj;
    matrix InvViewProj;
    float4 CameraNearFarPlanes;
    float4 VoxelSize;
    float BlendingWithSceneColorFactor;
};

// 노이즈 생성을 위한 함수
float GetProceduralNoiseSample(uint3 p)
{
    p = p * 1664525u + 1013904223u;
    p.x += p.y * p.z;
    p.y += p.z * p.x;
    p.z += p.x * p.y;
    p ^= p >> 16u;
    p.x += p.y * p.z;
    p.y += p.z * p.x;
    p.z += p.x * p.y;
    p ^= p >> 16u;
    return float(p.z) / 4294967295.0f;
}

float3 GetVolumetricFog(float3 inputColor, float3 worldPos, float nearPlane, float farPlane, float4 screenPos)
{
    float3 uv = GetUVFromVolumetricFogVoxelWorldPos(worldPos, nearPlane, farPlane, ViewProj, VoxelSize.xyz);
    
    // 최종 샘플링 시 Z좌표에 노이즈를 추가하여 슬라이스 경계면을 디더링
    float noise = GetProceduralNoiseSample(uint3(screenPos.xy, CameraNearFarPlanes.z)); // .z에 담긴 프레임 인덱스를 노이즈 시드로 사용
    uv.z += (noise - 0.5f) / VoxelSize.z; // 1 복셀 깊이만큼의 노이즈 추가
    
    float4 scatteredLight = fogGridTexture.SampleLevel(samLinear_clamp, uv, 0);
    return inputColor * scatteredLight.a + scatteredLight.rgb;
}

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 ps_main(PSInput input) : SV_Target0
{
    float4 res = float4(0, 0, 0, 1);
    float4 inputColor = float4(screenMap.SampleLevel(samLinear_wrap, input.uv, 0).xyz,1.f);
    float depth = depthMap.SampleLevel(samLinear_wrap, input.uv,0).r;
    float4 worldPos = float4(ReconstructWorldPos(input.uv, depth, InvViewProj), 1);
    
    if (depth == 1.0f)
        return inputColor;
    
    float3 color = GetVolumetricFog(inputColor.rgb / inputColor.a, worldPos.xyz / worldPos.w,
                                    CameraNearFarPlanes.x, CameraNearFarPlanes.y, input.position);
    return float4(lerp(inputColor.rgb, color, BlendingWithSceneColorFactor), 1.f);
}
