#include "CommonDefines.hlsli"
#include "CommonStructs.hlsli"
#include "CommonSamplers.hlsli"

ConstantBuffer<CameraData> cameraData : register(b0);
ConstantBuffer<LightData>  lightData : register(b1);
ConstantBuffer<NumLight>   bit32_3_numLight : register(b2);
ConstantBuffer<ObjectData> bit32_2_objectData : register(b3);