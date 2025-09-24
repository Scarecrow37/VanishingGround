#include "CommonData.hlsli"
#include "Function.hlsli"

RWTexture3D<float4> VoxelWriteTexture;
Texture3D<float4> VoxelReadTexture;
ConstantBuffer<VolumetricFogData> fogdata;
Texture2DArray<float> ShadowMap;

float Hash33(uint3 p)
{
    p = p * 1664525u + 1013904223u;
    p.x += p.y * p.z;
    p.y += p.z * p.x;
    p.z += p.x * p.y;
    return frac((p.x ^ p.y ^ p.z) * 0.0000001f);
}

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

float GetVisibillityCSM(float3 voxelWorldPos)
{
    // 거리 기반 cascade 선택 
    float dist = length(voxelWorldPos - cameraData.Position.xyz);
    uint cid = 2;
    
    if (dist < cascadeData.CascadeSplits.x)
        cid = 0;
    else if (dist < cascadeData.CascadeSplits.y)
        cid = 1;
    else
        cid = 2;
    
    float4 shadowPosLS = mul(float4(voxelWorldPos, 1.f), cascadeData.ShadowVP[cid]);
    
    if (abs(shadowPosLS.w) < Epsilon)
        return 0.f;
    
    shadowPosLS.xyz /= shadowPosLS.w;
    
    float2 shadowUV = shadowPosLS.xy * 0.5f + 0.5f;
    shadowUV.y = 1.f - shadowUV.y;
    
    if (shadowUV.x < 0.0f || shadowUV.x > 1.0f || shadowUV.y < 0.0f || shadowUV.y > 1.0f)
    {
        return 1.0f;
    }
    
    float2 texelSize = float2(1.f / 2048, 1.f / 2048);
    float bias = max(0.0005f, 0.00002f * dist);
    
    float sum = 0.f;
    for (int oy = -1; oy <= 1; ++oy)
    {
        for (int ox = -1; ox <= 1; ++ox)
        {
            float2 off = float2(ox, oy) * texelSize;
            sum += ShadowMap.SampleCmpLevelZero(samComparisonLinear_border, float3(shadowUV + off, float(cid)), shadowPosLS.z - bias);
        }
    }
    return sum / 9.0f;
    
}
[numthreads(8, 8, 1)]
void cs_main(uint3 Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID, uint3 DTid : SV_DispatchThreadID)
{
    uint3 texCoord = DTid.xyz;
    
    if (texCoord.x < fogdata.VolumeSize.x && texCoord.y < fogdata.VolumeSize.y && texCoord.z < fogdata.VolumeSize.z)
    {
        // 거리에 따라 지터링 강도 조절
        // 가까울수록 1.0, 멀어질수록 0.0에 가까워짐
        float depth_based_jitter_strength = 1.0f - saturate(float(texCoord.z) / fogdata.VolumeSize.z);
        depth_based_jitter_strength *= depth_based_jitter_strength; // 강도 변화를 더 급격하게 만듦

        float jitter = frac((GetProceduralNoiseSample(texCoord) - 0.5f) * (1.f - 0.000001) * 1) * depth_based_jitter_strength;

        float3 voxelWorldPos = GetWorldPosFromVoxelID(texCoord, jitter,
                            fogdata.CameraNearFar_FrameIndex_PreviousFrameBlend.x,
                            fogdata.CameraNearFar_FrameIndex_PreviousFrameBlend.y, fogdata.InverseViewProj,
                            fogdata.VolumeSize.xyz);
        float3 voxelWorldPosNoJitter = GetWorldPosFromVoxelID(texCoord, 0.0f,
                                    fogdata.CameraNearFar_FrameIndex_PreviousFrameBlend.x,
                                    fogdata.CameraNearFar_FrameIndex_PreviousFrameBlend.y,
                                    fogdata.InverseViewProj, fogdata.VolumeSize.xyz);
        
        float3 viewDir = normalize(cameraData.Position.xyz - voxelWorldPos);

        float3 lighting = float3(0.0, 0.0, 0.0);
        float visibility = GetVisibillityCSM(voxelWorldPos);
        float3 fogValue = lightData.Directional[0].Color.xyz *
                HenyeyGreensteinPhaseFunction(viewDir, -lightData.Directional[0].Direction.xyz, fogdata.LightShaftAnisotropy);
        // light shaft
        if (visibility > Epsilon)
            lighting += visibility * fogValue * fogdata.LightShaftIntensity;
        // fog
        float3 fixedLightDir = float3(0.0f, 1.0f, 0.0f);
        fogValue = fogdata.FogColor.xyz *
                HenyeyGreensteinPhaseFunction(viewDir, -fixedLightDir, fogdata.FogAnisotropy);
        lighting += fogValue * fogdata.FogIntensity;
        
        float4 result = float4(lighting * fogdata.Strength * fogdata.Density,  fogdata.Density);
        
        //previous frame interpolation
        {
            float3 prevUV = GetUVFromVolumetricFogVoxelWorldPos(voxelWorldPosNoJitter,
                                                fogdata.CameraNearFar_FrameIndex_PreviousFrameBlend.x,
                                                fogdata.CameraNearFar_FrameIndex_PreviousFrameBlend.y,
                                                fogdata.PrevViewProj, fogdata.VolumeSize.xyz);
        
            if (prevUV.x >= 0.0f && prevUV.y >= 0.0f && prevUV.z >= 0.0f &&
                prevUV.x <= 1.0f && prevUV.y <= 1.0f && prevUV.z <= 1.0f)
            {
                float4 prevResult = VoxelReadTexture.SampleLevel(samLinear_clamp, prevUV, 0.0f);
                result = lerp(result, prevResult, fogdata.CameraNearFar_FrameIndex_PreviousFrameBlend.z);
            }
        }

        VoxelWriteTexture[texCoord] = result;
    }
}
