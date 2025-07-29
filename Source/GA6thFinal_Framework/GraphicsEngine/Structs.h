#pragma once
#include "Graphics_Structs.h"

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
};

struct Resolution
{
    UINT Width;
    UINT Height;
};