#pragma once
#include "ResourceBase.h"

struct ShadowFaceRegion
{
    UINT X;
    UINT Y;
    UINT Width;
    UINT Height;
};

class PointLightShadowAtlas : public ResourceBase
{
public:
    static constexpr UINT FacePerLight           = 6;
    static constexpr UINT MAX_SHADOW_POINT_LIGHT = 10;

    void InitializeAtlas(UINT atlasSize, UINT faceSize);
    void ResizeFace(UINT newFaceSize);

    void AllocateLight(UINT lightIndex);
    void ReleaseLight(UINT lightIndex);
    bool IsAllocated(UINT lightIndex) const;

    DescriptorHandles GetDSVHandle() const;
    D3D12_VIEWPORT    GetViewport(UINT lightIndex, UINT faceIndex) const;
    D3D12_RECT        GetScissorRect(UINT lightIndex, UINT faceIndex) const;

private:
    void CreateAtlasResource();
    void BuildRegions();
    void CreateDSVHandles();

private:
    UINT                   _atlasSize = 8192;
    UINT                   _faceSize  = 1024;

    std::array<ShadowFaceRegion, FacePerLight * MAX_SHADOW_POINT_LIGHT>  _regions;
    DescriptorHandles _dsvHandle;

    std::array<bool, MAX_SHADOW_POINT_LIGHT> _allocated = {};
    std::vector<UINT>                        _freeList;
};
