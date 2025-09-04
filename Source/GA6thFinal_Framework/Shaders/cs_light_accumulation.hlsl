#include "CommonData.hlsli"
#include "Function.hlsli"
ConstantBuffer<VolumetricFogData> fogdata;
RWTexture3D<float4> VoxelWriteTexture;
Texture3D<float4> VoxelReadTexture;

float GetSliceDistance(int z, float near, float far)
{
    return near * pow(far / near, (float(z) + 0.5f) / fogdata.VolumeSize.z);
}
float GetSliceThickness(int z, float near, float far)
{
    return abs(GetSliceDistance(z + 1, near, far) - GetSliceDistance(z, near, far));
}

// https://github.com/Unity-Technologies/VolumetricLighting/blob/master/Assets/VolumetricFog/Shaders/Scatter.compute
/*
    z : 현재 z 슬라이스
    result : 이전 슬라이스까지 누적된 결과
    colorDensityPerSlice : 현재 슬라이스의 색상 및 밀도
*/
float4 Accumulate(int z, float4 result, float4 colorDensityPerSlice)
{
    // 최소 밀도 보정
    colorDensityPerSlice.a = max(colorDensityPerSlice.a, 0.000001f);
    // (원래 구현) 슬라이스 두계와 함께 Beer-Lambert 법칙 적용
    float thickness = GetSliceThickness(z, fogdata.CameraNearFar_FrameIndex_PreviousFrameBlend.x,
          fogdata.CameraNearFar_FrameIndex_PreviousFrameBlend.y);
    float sliceTransmittance = exp(-colorDensityPerSlice.a * thickness * fogdata.ThicknessFactor);

    // (수정 구현) Seb Hillaire 기법
    // Frostbite의 Sebastien Hillaire 논문에서 제안된 기법
    // 슬라이스 깊이에 대해 적분(integral)을 계산하는 방식
    // 각 슬라이스에서의 조명은 일정하다고 가정하지만, 깊이에 따라 정확한 투과율을 구함
    // http://www.frostbite.com/2015/08/physically-based-unified-volumetric-rendering-in-frostbite/
    //float sliceTransmittance = exp(-colorDensityPerSlice.a / fogdata.VolumeSize.z);
    // 현재 슬라이스에서 산란되는 빛 계산
    // (1 - sliceTransmittance): 흡수되지 않고 산란되는 부분
    // colorDensityPerSlice.a: 밀도로 나누어 에너지 보존(정규화)
    float3 sliceScattering = colorDensityPerSlice.rgb * (1.0f - sliceTransmittance) / colorDensityPerSlice.a;

    // 누적 색상에 이번 슬라이스 산란 값 추가
    // result.a: 지금까지 누적된 투과율
    result.rgb += sliceScattering * result.a;

    // 누적 투과율 갱신 (이번 슬라이스의 감쇠 적용)
    result.a *= sliceTransmittance;

    return result;
}

[numthreads(8, 8, 1)]
void cs_main(uint Gid : SV_GroupID, uint3 GTid : SV_GroupThreadID, uint3 DTid : SV_DispatchThreadID)
{
    float4 result = float4(0, 0, 0, 1);
    for (int z = 0; z < fogdata.VolumeSize.z;z++)
    {
        uint3 texCoord = uint3(DTid.x, DTid.y, z);
        float4 colorDensityPerSlice = VoxelReadTexture.Load(uint4(texCoord, 0));
        result = Accumulate(z, result, colorDensityPerSlice);
        VoxelWriteTexture[texCoord] = result;
    }

}