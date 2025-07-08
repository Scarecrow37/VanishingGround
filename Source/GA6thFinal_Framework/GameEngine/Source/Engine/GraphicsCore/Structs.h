#pragma once

struct Vertex
{
    Vector4 Position;
    Vector3 Normal;
    Vector3 Tangent;
    Vector3 BiTangent;
    Vector2 UV;
};

struct StaticMeshVertex
{
    Vector4 Position;
    Vector3 Normal;
    Vector3 Tangent;
    Vector3 BiTangent;
    Vector2 UV;
};

struct SkeletalMeshVertex : public Vertex
{
    UINT  BlendIndices[4]{};
    float BlendWeights[4]{};
};

struct BoneMatrices
{
    XMMATRIX matrix[MAX_BONE_MATRIX];
};

struct MaterialID
{
    UINT ID[4];
};

struct UIMaterial
{
    UINT ID;
    float Alpha;
};

using VertexBufferID = UINT;
using IndexBufferID = UINT;

struct Material
{
#undef OPAQUE
    enum class ShadingModel
    {
        UNLIT,
        DEFAULTLIT,
        END
    };
    enum class BlendMode
    {
        OPAQUE,
        MASKED,
        TRANSLUCENT,
        ADDITIVE,
        MODULATE,
        END
    };
    ShadingModel Model;
    BlendMode    Mode;
    bool         IsTwoSided;
};

struct CameraData
{
    XMMATRIX View;
    XMMATRIX Projection;
    XMMATRIX ViewInverse;
    XMMATRIX ProejctionInverse;
    Vector4  Position{0.f, 0.f, 0.f, 1.f};
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

struct NumLight
{
    unsigned int Directional;
    unsigned int Point;
    unsigned int Spot;
};

struct DescriptorHandles
{
    D3D12_CPU_DESCRIPTOR_HANDLE CPU;
    D3D12_GPU_DESCRIPTOR_HANDLE GPU;
};

struct PostProcessData
{
    Vector2      ScreenSize;
    Vector2      TexelSize;
    unsigned int PostProcessMask;
};

class UnorderedAccessView;
struct SkeletalMeshInstance
{
    std::shared_ptr<UnorderedAccessView> UAVBuffer;
    D3D12_VERTEX_BUFFER_VIEW             VertexBufferView;
    UINT                                 VertexCount = 0;
};