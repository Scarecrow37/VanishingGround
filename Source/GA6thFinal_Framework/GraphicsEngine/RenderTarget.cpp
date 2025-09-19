#include "pch.h"
#include "RenderTarget.h"

void RenderTarget::Initialize(const D3D12_RESOURCE_DESC& desc, FLOAT clearColor)
{
    _clearValue = {clearColor, clearColor, clearColor, 1.f};

    _desc = desc;

    _currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

    _resolution = {(LONG)desc.Width, (LONG)desc.Height};

    UINT mipLevelCount = 1;

    if (_desc.MipLevels != 1)
    {
        UINT maxValue = std::max((UINT)desc.Width, desc.Height);
        while (maxValue > 0)
        {
            maxValue /= 2;
            mipLevelCount++;
        }
    }

    _rtvHandles.resize(mipLevelCount);
    _viewPorts.resize(mipLevelCount);
    _scissorRects.resize(mipLevelCount);

    for (UINT i = 0; i < mipLevelCount; i++)
    {
        _viewPorts[i]    = {.TopLeftX = 0.f,
                            .TopLeftY = 0.f,
                            .Width    = (FLOAT)(desc.Width >> i),
                            .Height   = (FLOAT)(desc.Height >> i),
                            .MinDepth = 0.f,
                            .MaxDepth = 1.f};

        _scissorRects[i] = {.left = 0, 
                            .top = 0, 
                            .right = (LONG)(desc.Width >> i), 
                            .bottom = (LONG)(desc.Height >> i)};

        Global::viewManager->AddDescriptorHeap(ViewManager::Type::RENDER_TARGET, _rtvHandles[i]);
    }

    CreateRenderTargetView();

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _srvHandle, &_ID);
    CreateShaderResourceView();
}

void RenderTarget::Initialize(const D3D12_RESOURCE_DESC& desc, FLOAT clearColor[4])
{
    _clearValue = {clearColor[0], clearColor[1], clearColor[2], clearColor[3]};

    _desc = desc;

    _currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

    _resolution = {(LONG)desc.Width, (LONG)desc.Height};

    UINT mipLevelCount = 1;

    if (_desc.MipLevels != 1)
    {
        UINT maxValue = std::max((UINT)desc.Width, desc.Height);
        while (maxValue > 0)
        {
            maxValue /= 2;
            mipLevelCount++;
        }
    }

    _rtvHandles.resize(mipLevelCount);
    _viewPorts.resize(mipLevelCount);
    _scissorRects.resize(mipLevelCount);

    for (UINT i = 0; i < mipLevelCount; i++)
    {
        _viewPorts[i] = {.TopLeftX = 0.f,
                         .TopLeftY = 0.f,
                         .Width    = (FLOAT)(desc.Width >> i),
                         .Height   = (FLOAT)(desc.Height >> i),
                         .MinDepth = 0.f,
                         .MaxDepth = 1.f};

        _scissorRects[i] = {.left = 0, .top = 0, .right = (LONG)(desc.Width >> i), .bottom = (LONG)(desc.Height >> i)};

        Global::viewManager->AddDescriptorHeap(ViewManager::Type::RENDER_TARGET, _rtvHandles[i]);
    }

    CreateRenderTargetView();

    Global::viewManager->AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _srvHandle, &_ID);
    CreateShaderResourceView();
}

void RenderTarget::CreateRenderTargetView()
{        
    CD3DX12_HEAP_PROPERTIES property(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_CLEAR_VALUE clearValue{.Format = _desc.Format,
                                 .Color  = {_clearValue.x, _clearValue.y, _clearValue.z, _clearValue.w}};

    ID3D12Device* device = Global::device->GetDevice();
    HRESULT       hr     = S_OK;

    hr = device->CreateCommittedResource(&property, D3D12_HEAP_FLAG_NONE, &_desc, D3D12_RESOURCE_STATE_RENDER_TARGET, &clearValue, IID_PPV_ARGS(&_resource));
    FAILED_CHECK_MESSAGE(hr, L"RenderTarget::Initialize CreateCommittedResource Failed");
    _desc = _resource->GetDesc();

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format                        = _desc.Format;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

    for (UINT i = 0; i < _desc.MipLevels; i++)
    {
        rtvDesc.Texture2D.MipSlice = i;
        device->CreateRenderTargetView(_resource.Get(), &rtvDesc, _rtvHandles[i]);
    }
}

void RenderTarget::CreateShaderResourceView()
{   
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format                          = _desc.Format;
    srvDesc.ViewDimension                   = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip       = 0;
    srvDesc.Texture2D.MipLevels             = _desc.MipLevels;
    srvDesc.Shader4ComponentMapping         = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

    Global::device->GetDevice()->CreateShaderResourceView(_resource.Get(), &srvDesc, _srvHandle.CPU);
}

void RenderTarget::ClearRenderTarget(ID3D12GraphicsCommandList* commandList, UINT mipLevel)
{    
    commandList->ClearRenderTargetView(_rtvHandles[mipLevel], _clearValue, 0, nullptr);
}

void RenderTarget::ResizeResource(SIZE resolution)
{
    _currentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

    _desc.Width = resolution.cx;
    _desc.Height = resolution.cy;

    for (UINT i = 0; i < _rtvHandles.size(); i++)
    {
        _viewPorts[i]    = {.TopLeftX = 0.f,
                            .TopLeftY = 0.f,
                            .Width    = (FLOAT)(resolution.cx >> i),
                            .Height   = (FLOAT)(resolution.cy >> i),
                            .MinDepth = 0.f,
                            .MaxDepth = 1.f};

        _scissorRects[i] = {.left = 0, 
                            .top = 0, 
                            .right = (LONG)(resolution.cx >> i), 
                            .bottom = (LONG)(resolution.cy >> i)};
    }

    _resolution = resolution;

    CreateRenderTargetView();
    CreateShaderResourceView();
}