#pragma once
#include "ResourceBase.h"

struct ShadowFaceRegion
{
    UINT X;
    UINT Y;
    UINT Width;
    UINT Height;
};

// PointLightShadowAtlas.h (중략)
class PointLightShadowAtlas
{
public:
    static constexpr UINT FacePerLight           = 6;
    static constexpr UINT MAX_SHADOW_POINT_LIGHT = 10;

    void InitializeAtlas(UINT atlasSize, UINT faceSize);
    void ResizeFace(UINT newFaceSize);

    UINT AllocateLight();
    void ReleaseLight(UINT lightIndex);
    bool IsAllocated(UINT lightIndex) const;

    DescriptorHandles GetDSVHandles(UINT lightIndex, UINT faceIndex) const;
    D3D12_VIEWPORT    GetViewport(UINT lightIndex, UINT faceIndex) const;
    D3D12_RECT        GetScissorRect(UINT lightIndex, UINT faceIndex) const;

private:
    void CreateAtlasResource();
    void BuildRegions();
    void CreateDSVHandles();

private:
    UINT                   _atlasSize = 8192;
    UINT                   _faceSize  = 1024;
    ComPtr<ID3D12Resource> _resource;

    std::array<ShadowFaceRegion, FacePerLight * MAX_SHADOW_POINT_LIGHT>  _regions;
    std::array<DescriptorHandles, FacePerLight * MAX_SHADOW_POINT_LIGHT> _dsvHandles;

    std::array<bool, MAX_SHADOW_POINT_LIGHT> _allocated = {};
    std::vector<UINT>                        _freeList;
};
