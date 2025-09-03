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

struct MaterialID
{
    UINT ID[4];
};

struct UIMaterial
{
    UINT ID;
    float Alpha;
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
    Vector4  CameraNearFar_FrameIndex_PreviousFrameBlend; // x=near, y=far, z=frameIndex, w=prevBlend
    Vector4   VolumeSize;                                  // x=volX, y=volY, z=volZ
    float    Anisotropy;
    float    Density;
    float    Strength;
    float    ThicknessFactor;
};

struct VolumetricFogCompositeData
{
    XMMATRIX ViewProj;
    XMMATRIX InverseViewProjection;
    Vector4 CameraNearFar;
    Vector4 VoxelSize;
    float   BlendWithScene;
};

struct NumLight
{
    unsigned int Directional;
    unsigned int Point;
    unsigned int Spot;
};

struct PostProcessData
{
    Vector2      ScreenSize;
    Vector2      TexelSize;
    unsigned int PostProcessMask;
    unsigned int MipLevel;
};

struct Resolution
{
    UINT Width;
    UINT Height;
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