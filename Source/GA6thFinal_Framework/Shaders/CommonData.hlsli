#ifndef __COMMON_DATA__
#define __COMMON_DATA__

#include "CommonDefines.hlsli"
#include "CommonStructs.hlsli"
#include "CommonSamplers.hlsli"

ConstantBuffer<CameraData> cameraData : register(b0);
ConstantBuffer<LightData>  lightData : register(b1);
ConstantBuffer<NumLight>   bit32_3_numLight : register(b2);
ConstantBuffer<ObjectData> bit32_3_objectData : register(b3);
ConstantBuffer<PostProcessData> bit32_5_postProcessData : register(b4);

#endif