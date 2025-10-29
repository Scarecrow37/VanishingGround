#ifndef __COMMON_STRUCTS__
#define __COMMON_STRUCTS__

#define MAX_DIRECTIONAL_LIGHT 4
#define MAX_POINT_LIGHT 64
#define MAX_SPOT_LIGHT 16
#define MAX_SHADOW_POINT_LIGHT 10
#define MAX_BONE_MATRIX 256
#define MAX_CASCADES 3

struct InstanceData
{
    uint4 MaterialID;
    uint  MatrixID;
    uint  CustomDepth;
    float Alpha;
};

struct MatrixData
{
    matrix World;
    matrix InverseTranspose;
};

struct DirectionalLight
{
    float3 Color;
    float Intensity;
    float3 Direction;
    float Padding1;
    float3 Ambient;
    float Padding2;
    float4 Padding3;
};

struct PointLight
{
    float3 Color;
    float Intensity;
    float3 Position;
    float Range;
    float3 Attenuation;
    float Padding1;
    float4 Padding2;
};

struct SpotLight
{
    float3 Color;
    float Intensity;
    float3 Position;
    float Range;
    float3 Direction;
    float InnerCone;
    float3 Attenuation;
    float OuterCone;
};

struct NumLight
{
    uint Directional;
    uint Point;
    uint Spot;
    uint ShadowPoint;
};

struct CameraData
{
    matrix View;
    matrix Projection;
    matrix ViewInverse;
    matrix ProjectionInverse;
    float4 Position;
};

struct LightData
{
    DirectionalLight Directional[MAX_DIRECTIONAL_LIGHT];
    PointLight Point[MAX_POINT_LIGHT];
    SpotLight Spot[MAX_SPOT_LIGHT];
    PointLight ShadowPoint[MAX_SHADOW_POINT_LIGHT];
};

struct PostProcessData
{
    float2 ScreenSize;
    float2 TexelSize;
    uint PostProcessMask;
    uint MipLevel;
};

struct CascadeData
{
    matrix ShadowVP[MAX_CASCADES + 1]; // 0,1,2: cascade shadows, 3: skeletal shadow
    float3 CascadeSplits; // x=split1, y=split2, z=split3
};

struct GbufferData
{
    float HeightScale;
    float MipBias;
};

struct VolumetricFogData
{
    float4x4 PrevViewProj;
    float4x4 InverseViewProj;
    float4 CameraNearFar_FrameIndex_PreviousFrameBlend; // x=near, y=far, z=frameIndex, w=prevBlend
    float4 VolumeSize; // x=volX, y=volY, z=volZ
    float4 FogColor;
    float FogAnisotropy;
    float LightShaftAnisotropy;
    float Density;
    float Strength;
    float ThicknessFactor;
    float FogIntensity;
    float LightShaftIntensity;
};

struct SSGIData
{
    float4x4 PreViewProj;
    float4x4 InverseViewProjection;
    float2 ScreenSize;
    float Radius;
    float Thickness;
    int NumSample;
    float Intensity;
    float TemporalWeight;
    float DepthSigma;
    float NormalSigma;
};

struct FXAAData
{
    float2 InverseResolution;
    float QualitySubpixel;
    float QualityEdgeDetectionThreshold;
    float QualityMinimumEdgeThreshold;
};

struct OITNode
{
    float4 Color;
    float Depth;
    uint Next;
};

struct PointLightShadowData
{
    float4x4 ViewProjection[6];
    float3 LightPosition;
    float FarPlane;
};

#endif