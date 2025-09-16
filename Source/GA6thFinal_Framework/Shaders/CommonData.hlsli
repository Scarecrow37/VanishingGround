#ifndef __COMMON_DATA__
#define __COMMON_DATA__

#include "CommonDefines.hlsli"
#include "CommonStructs.hlsli"
#include "CommonSamplers.hlsli"

ConstantBuffer<CameraData> cameraData : register(b0);
ConstantBuffer<CascadeData> cascadeData : register(b1);
ConstantBuffer<LightData> lightData : register(b2);
ConstantBuffer<NumLight> bit32_3_numLight : register(b3);
ConstantBuffer<ObjectData> bit32_3_objectData : register(b4);
ConstantBuffer<ShadowObjectData> bit32_4_shadowObjectData : register(b5);
ConstantBuffer<TranslucentObjectData> bit32_4_translucentObjectData : register(b6);
ConstantBuffer<PostProcessData> bit32_6_postProcessData : register(b7);
    
static const float GaussianWeight[] =
{
    0.013519569015984728,
    0.047662179108871855,
    0.11723004402070096,
    0.20116755999375591,
    0.240841295721373,
    0.20116755999375591,
    0.11723004402070096,
    0.047662179108871855,
    0.013519569015984728
};

//static const float GaussianWeight[] =
//{
//    0.004429, 0.008118, 0.014273, 0.023234, 0.034757,
//    0.047816, 0.060598, 0.070920, 0.076945, 0.077292,
//    0.071036, 0.059066, 0.043232, 0.026596, 0.012740
//};

#endif