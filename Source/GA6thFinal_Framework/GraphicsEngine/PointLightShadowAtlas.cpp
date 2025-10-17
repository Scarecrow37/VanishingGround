#include "pch.h"
#include "PointLightShadowAtlas.h"

void PointLightShadowAtlas::InitializeAtlas(UINT atlasSize, UINT faceSize)
{
    _atlasSize = atlasSize;
    _faceSize  = faceSize;
    CreateAtlasResource();
    BuildRegions();
    CreateDSVHandles();
}

void PointLightShadowAtlas::ResizeFace(UINT newFaceSize) 
{
    if (_faceSize == newFaceSize)
        return; // 변경 없음

    _faceSize = newFaceSize;

    // 기존 DSV 해제 후 재생성
    _dsvHandles.fill({});
    BuildRegions();
    CreateDSVHandles();
}

UINT PointLightShadowAtlas::AllocateLight()
{
    if (_freeList.empty())
    {
        return UINT_MAX;
    }
    UINT index = _freeList.back();
    _freeList.pop_back();

    GRAPHICS_ASSERT(index < MAX_SHADOW_POINT_LIGHT, L"PointLightShadowAtlas::AllocateLight : Light Index Out of Range");
    _allocated[index] = true;

    return index;
}

void PointLightShadowAtlas::ReleaseLight(UINT lightIndex) 
{
    GRAPHICS_ASSERT(lightIndex < MAX_SHADOW_POINT_LIGHT, L"ReleaseLight : Light Index Out of Range");

    if (!_allocated[lightIndex])
    {
        return;
    }

    _allocated[lightIndex] = false;

    _freeList.push_back(lightIndex);
}

bool PointLightShadowAtlas::IsAllocated(UINT lightIndex) const
{
    if (lightIndex >= MAX_SHADOW_POINT_LIGHT)
        return false;
    return _allocated[lightIndex];
}

DescriptorHandles PointLightShadowAtlas::GetDSVHandles(UINT lightIndex, UINT faceIndex) const
{
    UINT index = lightIndex * FacePerLight + faceIndex;
    GRAPHICS_ASSERT(index < FacePerLight * MAX_SHADOW_POINT_LIGHT,
                    L"PointLightShadowAtlas::GetDSVHandles : Index out of range.");
    return _dsvHandles[index];
}

D3D12_VIEWPORT PointLightShadowAtlas::GetViewport(UINT lightIndex, UINT faceIndex) const
{
    GRAPHICS_ASSERT(lightIndex < MAX_SHADOW_POINT_LIGHT,
                    L"PointLightShadowAtlas::GetViewports : Light Index Out of Range");
    GRAPHICS_ASSERT(faceIndex < FacePerLight, L"PointLightShadowAtlas::GetViewports : Face Index Out of Range");

    const ShadowFaceRegion& region   = _regions[lightIndex * FacePerLight + faceIndex];
    D3D12_VIEWPORT          viewport = {};
    viewport.TopLeftX                = static_cast<FLOAT>(region.X);
    viewport.TopLeftY                = static_cast<FLOAT>(region.Y);
    viewport.Width                   = static_cast<FLOAT>(region.Width);
    viewport.Height                  = static_cast<FLOAT>(region.Height);
    viewport.MinDepth                = 0.0f;
    viewport.MaxDepth                = 1.0f;
    return viewport;
}

D3D12_RECT PointLightShadowAtlas::GetScissorRect(UINT lightIndex, UINT faceIndex) const
{
    GRAPHICS_ASSERT(lightIndex < MAX_SHADOW_POINT_LIGHT,
                    L"PointLightShadowAtlas::GetScissorRect : Light Index Out of Range");
    GRAPHICS_ASSERT(faceIndex < FacePerLight, L"PointLightShadowAtlas::GetScissorRect : Face Index Out of Range");
    const ShadowFaceRegion& region = _regions[lightIndex * FacePerLight + faceIndex];
    D3D12_RECT              rect   = {(LONG)region.X, (LONG)region.Y, (LONG)(region.X + region.Width),
                                      (LONG)(region.Y + region.Height)};
    return rect;
}

void PointLightShadowAtlas::CreateAtlasResource()
{
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension           = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width               = _atlasSize;
    texDesc.Height              = _atlasSize;
    texDesc.DepthOrArraySize    = 1;
    texDesc.MipLevels           = 1;
    texDesc.Format              = DXGI_FORMAT_D32_FLOAT;
    texDesc.SampleDesc.Count    = 1;
    texDesc.Flags               = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE clearValue  = {};
    clearValue.Format             = DXGI_FORMAT_D32_FLOAT;
    clearValue.DepthStencil.Depth = 1.0f;

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

    HRESULT hr = Global::device->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &texDesc,
                                                                      D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue,
                                                                      IID_PPV_ARGS(&_resource));
    GRAPHICS_ASSERT(SUCCEEDED(hr),
                    L"PointLightShadowAtlas::CreateAtlasResource : Failed to create shadow atlas resource.");
}

void PointLightShadowAtlas::BuildRegions() 
{
    const UINT tilesPerRow = _atlasSize / _faceSize;
    const UINT totalFaces  = FacePerLight * MAX_SHADOW_POINT_LIGHT;
    UINT       x = 0, y = 0;
    for (UINT i = 0; i < totalFaces; ++i)
    {
        _regions[i] = {x * _faceSize, y * _faceSize, _faceSize, _faceSize};
        ++x;
        if (x >= tilesPerRow)
        {
            x = 0;
            ++y;
        }
    }
}

void PointLightShadowAtlas::CreateDSVHandles() 
{
    const UINT totalFaces = FacePerLight * MAX_SHADOW_POINT_LIGHT;

    for (UINT i = 0; i < totalFaces; ++i)
    {
        DescriptorHandles dsvHandle;
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::DEPTH_STENCIL, dsvHandle);

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format                        = DXGI_FORMAT_D32_FLOAT;
        dsvDesc.ViewDimension                 = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Flags                         = D3D12_DSV_FLAG_NONE;

        Global::device->GetDevice()->CreateDepthStencilView(_resource.Get(), &dsvDesc, dsvHandle.CPU);

        _dsvHandles[i] = dsvHandle;
    }
}

