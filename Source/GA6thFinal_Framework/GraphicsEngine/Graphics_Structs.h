#pragma once

using VertexBufferID = UINT;
using IndexBufferID  = UINT;
using MeshInstanceID = UINT;

struct Vertex
{
    Vector4 Position;
    Vector2 UV;
};

struct StaticMeshVertex
{
    Vector4 Position;
    Vector3 Normal;
    Vector3 Tangent;
    Vector3 BiTangent;
    Vector2 UV[2];
};

struct SkeletalMeshVertex : public StaticMeshVertex
{
    UINT  BlendIndices[4]{};
    float BlendWeights[4]{};
};

struct Material
{
#undef OPAQUE    
    enum ShadingModelType
    {
        // UNLIT,
        DEFAULTLIT,
        CUSTOMLIT,
        SMT_END
    } ShadingModel{};
    enum BlendModeType
    {
        OPAQUE,
        MASKED,
        TRANSLUCENT,
        BMT_END
    } BlendMode{};
    enum CullModeType
    {
        CULL_BACK,
        CULL_FRONT,
        CULL_NONE
    } CullMode{};
    float Alpha = 1.f;
    bool IsTwoSided = false;
};

struct TransparentRimLightMaterial
{
    std::array<float, 3> RimColor     = {1.f, 1.f, 1.f};
    float                RimPower     = 1.f;
    float                RimIntensity = 1.f;
};

struct DescriptorHandles
{
    D3D12_CPU_DESCRIPTOR_HANDLE CPU;
    D3D12_GPU_DESCRIPTOR_HANDLE GPU;
};

struct SkeletalMeshInstance
{
    std::shared_ptr<class UnorderedAccessView> UAVBuffer;
    D3D12_VERTEX_BUFFER_VIEW                   VertexBufferView;
    UINT                                       VertexCount = 0;
};

struct LightData
{
    Vector3 Color;
    float   Intensity;
    Vector3 float3_1;
    float   float_1;
    Vector3 float3_2;
    float   float_2;
    Vector3 float3_3;
    float   float_3;
};

struct UIMaterialData
{
    UINT  Type;
    float Fill;
};

struct ShadowPassProperty
{
    float NearPlane;
    float FarPlane;
    float SplitFactor;
};

struct BloomPassProperty
{
    float Threshold;
    float Intensity;
    float BloomKnee;
};

struct ToneMappingProperty
{
    Vector3 WhiteBalance;
    float   Exposure;
    float   Saturation;
    float   Contrast;
};

struct SSAOPassProperty
{
    float Radius;
    float Falloff;
    float StrengthFactor;
    float ContrastFactor;
    float Threshold;
};

struct SSRPassProperty
{
    float MaxThickness;
    float StepSize;
    float MaxRayCount;
    float ScreenFade;
};

struct ParallaxMappingProperty
{
    float HeightScale;
    float MipBias;
};

struct VolumetricFogProperty
{
    float FogAnisotropy;
    float LightShaftAnisotropy;
    float Density;
    float Strength;
    float BlendWithScene;
    float BlendWithPrevFrame;
    float CustomNear;
    float CustomFar;
    float FogIntensity;
    float LightShaftIntensity;
    float FogColor[4];
};

struct SSGIProperty
{
    float Radius;//0.1~4.0
    float Thickness;//0.01~0.15
    int   NumSample;//8~16
    float Intensity;      // 0~2.0;
    float TemporalWeight;//이전프레임 가중치 //0.7~0.95
    float DepthSigma;//0.5~5.0
    float NormalSigma;//16~256
};

struct FXAAProperty
{
    float   QualitySubpixel;
    float   QualityEdgeDetectionThreshold;
    float   QualityMinimumEdgeThreshold;
};