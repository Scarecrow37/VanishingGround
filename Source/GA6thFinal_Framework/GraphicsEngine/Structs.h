#pragma once
#include "Graphics_Structs.h"

struct MatrixData
{
    Matrix World;
    Matrix InverseTransposeWorld;
};

struct BoneMatrices
{
    Matrix matrix[MAX_BONE_MATRIX];
};

struct InstanceData
{
    UINT  MaterialID[4];
    UINT  MatrixID;
    UINT  CustomDepth;
    FLOAT Alpha;
};

class DXRSkeletalMesh;
class BaseMesh;
class MeshRenderer;
struct MeshInfo
{
    InstanceData     InstanceData;
    Material         Material;
    BaseMesh*        Mesh;
    MeshRenderer*    Renderer;
    DXRSkeletalMesh* SkinnedInstance;
    Matrix*          TransposeWorldMatrix;
    float            DepthKey;
};

struct UIMaterial
{
    UINT  ID;
    FLOAT Alpha;
    UINT  NumColmn;
    UINT  NumRow;
    UINT  ColumnIndex;
    UINT  RowIndex;
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

struct CascadeData
{
    Matrix ShadowVP[MAX_CASCADES];
    float  CascadeSplits[MAX_CASCADES];
};

struct VolumetricFogData
{
    XMMATRIX PreViewProjection;
    XMMATRIX InverseViewProjection;
    Vector4  CameraNearFar_PreviousFrameBlend; // x = near, y = far, z=prevBlend , w = padding
    Vector4  VolumeSize;                       // x = volX, y = volY, z = volZ
    Vector4  FogColor;
    float    FogAnisotropy;
    float    LightShaftAnisotropy;
    float    Density;
    float    Strength;
    float    ThicknessFactor;
    float    FogIntensity;
    float    LightShaftIntensity;
};

struct SSGIData
{
    XMMATRIX PreViewProj;
    XMMATRIX InverseViewProjection;
    Vector2  ScreenSize;
    float    Radius;
    float    Thickness;
    int      NumSample;
    float    Intensity;
    float    TemporalWeight;
    float    DepthSigma;
    float    NormalSigma;
};

struct VolumetricFogCompositeData
{
    XMMATRIX ViewProj;
    XMMATRIX InverseViewProjection;
    Vector4  CameraNearFar;
    Vector4  VoxelSize;
    float    BlendWithScene;
};

struct FXAAData
{
    Vector2 InverseResolution;
    float  QualitySubpixel;
    float  QualityEdgeDetectionThreshold;
    float  QualityMinimumEdgeThreshold;
};

struct GBufferData
{
    float HeightScale;
    float MipBias;
};

struct NumLight
{
    unsigned int Directional;
    unsigned int Point;
    unsigned int Spot;
    unsigned int ShadowPoint;
};

struct PostProcessData
{
    Vector2      ScreenSize;
    Vector2      TexelSize;
    unsigned int PostProcessMask;
    unsigned int MipLevel;
};

struct PipelineStateStream
{
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE        RootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT          InputLayout;
    CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
    CD3DX12_PIPELINE_STATE_STREAM_VS                    VS;
    CD3DX12_PIPELINE_STATE_STREAM_GS                    GS;
    CD3DX12_PIPELINE_STATE_STREAM_PS                    PS;
    CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER            RasterizerState;
    CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT  DSVFormat;
    CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL         DepthStencilState;
    CD3DX12_PIPELINE_STATE_STREAM_BLEND_DESC            BlendState;
};

struct ComputePipelineStateStream
{
    CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE        RootSignature;
    CD3DX12_PIPELINE_STATE_STREAM_CS                    CS;
};

struct SDFParams
{
    unsigned int InstanceID;
    unsigned int Flags;
    float        DistanceRange;
    float        FontWeight;
    Vector4      OutlineColor;
    float        OutlineWidth;
};