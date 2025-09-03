#include "CommonData.hlsli"
#include "Function.hlsli"

Texture2D<float4> screenMap;
Texture2D<float> depthMap;
Texture3D<float4> fogGridTexture;

cbuffer VolumetricFogCompositeData
{
    float4x4 ViewProj;
    float4x4 InvViewProj;
    float4 CameraNearFarPlanes;
    float4 VoxelSize;
    float BlendingWithSceneColorFactor;
};

float3 GetVolumetricFog(float3 inputColor, float3 worldPos, float nearPlane, float farPlane)
{
    float3 uv = GetUVFromVolumetricFogVoxelWorldPos(worldPos, nearPlane, farPlane, ViewProj, VoxelSize.xyz);
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
                                    CameraNearFarPlanes.x, CameraNearFarPlanes.y);
    return float4(lerp(inputColor.rgb, color, BlendingWithSceneColorFactor), 1.f);
}