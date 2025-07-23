#pragma once

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

struct BoneMatrices
{
    Matrix matrix[MAX_BONE_MATRIX];
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
using StaticMeshInstanceID = UINT;
using SkeletalMeshInstanceID = UINT;


struct Material
{
#undef OPAQUE
    enum class ShadingModel
    {
        UNLIT,
        DEFAULTLIT,
        END
    } Model;
    enum class BlendMode
    {
        OPAQUE,
        MASKED,
        TRANSLUCENT,
        ADDITIVE,
        MODULATE,
        END
    } Mode;
    enum
    {
        CULL_BACK,
        CULL_FRONT,
        CULL_NONE
    } CullMode;
    bool         IsTwoSided;
};

struct CameraData
{
    XMMATRIX View;
    XMMATRIX Projection;
    XMMATRIX ViewInverse;
    XMMATRIX ProjectionInverse;
    Vector4  Position{0.f, 0.f, 0.f, 1.f};
};

struct RayCameraData
{
    XMMATRIX View;
    XMMATRIX Projection;
    XMMATRIX ViewInverse;
    XMMATRIX ProejctionInverse;
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

struct Resolution
{
    UINT Width;
    UINT Height;
};

struct MeshInfo
{
    Material        Material;
    class BaseMesh* Mesh;
    UINT            CustomDepth;
};

struct GraphicsTransform
{
    const Vector3&    Position;
    const Vector3&    Scale;
    const Quaternion& Rotation;
    const Matrix&     World;
};