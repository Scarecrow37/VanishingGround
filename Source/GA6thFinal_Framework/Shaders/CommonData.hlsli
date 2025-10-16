#ifndef __COMMON_DATA__
#define __COMMON_DATA__

#include "CommonDefines.hlsli"
#include "CommonStructs.hlsli"
#include "CommonSamplers.hlsli"

ConstantBuffer<CameraData> cameraData : register(b0);
ConstantBuffer<CascadeData> cascadeData : register(b1);
ConstantBuffer<LightData> lightData : register(b2);
ConstantBuffer<NumLight> bit32_4_numLight : register(b3);
ConstantBuffer<PostProcessData> bit32_6_postProcessData : register(b4);

StructuredBuffer<MatrixData> matrices;
StructuredBuffer<InstanceData> instanceData;

// Gaussian weights
// 5-tab
static const float GaussianWeight[] =
{
    0.06136, 0.24477, 0.38774, 0.24477, 0.06136
};

// 7-tab
//static const float GaussianWeight[] =
//{
//    0.028002, 0.065984, 0.121703, 0.159576, 0.121703, 0.065984, 0.028002
//};
// 9-tab
//static const float GaussianWeight[] =
//{
//    0.013519569015984728,
//    0.047662179108871855,
//    0.11723004402070096,
//    0.20116755999375591,
//    0.240841295721373,
//    0.20116755999375591,
//    0.11723004402070096,
//    0.047662179108871855,
//    0.013519569015984728
//};

// 15-tab
//static const float GaussianWeight[] =
//{
//    0.004429, 0.008118, 0.014273, 0.023234, 0.034757,
//    0.047816, 0.060598, 0.070920, 0.076945, 0.077292,
//    0.071036, 0.059066, 0.043232, 0.026596, 0.012740
//};


static const float3 RandomKernel[16] =
{
    float3(0.587, 0.245, 0.423), float3(-0.348, 0.378, 0.671),
    float3(0.234, -0.156, 0.854), float3(-0.479, 0.632, -0.124),
    float3(0.137, 0.892, -0.307), float3(-0.642, 0.187, 0.374),
    float3(0.214, -0.368, -0.784), float3(0.543, -0.732, 0.120),
    float3(-0.287, -0.476, 0.673), float3(0.726, -0.238, -0.304),
    float3(-0.123, 0.473, -0.789), float3(0.349, -0.832, 0.234),
    float3(-0.586, 0.125, 0.598), float3(0.142, 0.456, 0.759),
    float3(-0.437, -0.523, -0.312), float3(0.672, 0.374, 0.192)
};

#endif