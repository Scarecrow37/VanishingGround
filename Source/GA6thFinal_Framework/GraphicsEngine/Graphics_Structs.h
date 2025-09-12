#pragma once

using VertexBufferID         = UINT;
using IndexBufferID          = UINT;
using MeshInstanceID   = UINT;

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
    enum class ShadingModelType
    {
        UNLIT,
        DEFAULTLIT,
        END
    } ShadingModel;
    enum class BlendModeType
    {
        OPAQUE,
        MASKED,
        TRANSLUCENT,
        END
    } BlendMode;
    enum class CullModeType
    {
        CULL_BACK,
        CULL_FRONT,
        CULL_NONE
    } CullMode;
    bool IsTwoSided;
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
class DXRSkeletalMesh;
struct MeshInfo
{
    Material        Material;
    class BaseMesh* Mesh;
    UINT            CustomDepth;
    UINT            InstanceID;
    Matrix*        TransposeWorldMatrix;
    DXRSkeletalMesh* SkinnedInstance;
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

struct GraphicsTransform
{
    const Vector3&    Position;
    const Vector3&    Scale;
    const Quaternion& Rotation;
    const Matrix&     World;
    const bool&       IsDirtyFlag;
};

struct ShadowPassProperty
{
    bool operator==(const ShadowPassProperty& other) const
    {
        return NearPlane == other.NearPlane && 
               FarPlane == other.FarPlane && 
               SplitFactor == other.SplitFactor && 
               Offset1 == other.Offset1 &&
               Offset2 == other.Offset2;
    }

    float NearPlane;
    float FarPlane;
    float SplitFactor;
    float Offset1;
    float Offset2;
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

struct RimLightPassProperty
{
    Vector3 Color;
    float   Power;
    float   Intensity;
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
};