#include "pch.h"
#include "ViewManager.h"

void ViewManager::Initialize()
{
    HRESULT                    hr     = S_OK;
    ID3D12Device*              device = UmDevice.GetDevice();
    D3D12_DESCRIPTOR_HEAP_DESC desc{};

    desc.NumDescriptors = 1000;
    desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

    hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_shaderResourceHeap));
    FAILED_CHECK_MESSAGE(hr, L"ViewManager::Initialize Failed");

    desc.NumDescriptors = 100;
    desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_renderTargetHeap));
    FAILED_CHECK_MESSAGE(hr, L"ViewManager::Initialize Failed");

    desc.NumDescriptors = 10;
    desc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    desc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&_depthStencilHeap));
    FAILED_CHECK_MESSAGE(hr, L"ViewManager::Initialize Failed");

    _shaderResourceDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    _renderTargetDescriptorSize   = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    _depthStencilDescriptorSize   = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

void ViewManager::AddDescriptorHeap(const ViewManager::Type type, D3D12_CPU_DESCRIPTOR_HANDLE& handle)
{
    UINT offset = 0;

    switch (type)
    {
    case ViewManager::Type::SHADER_RESOURCE:
        offset = _shaderResourceDescriptorSize * _numShaderResource;
        handle = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += offset;
        _numShaderResource++;
        break;

    case ViewManager::Type::RENDER_TARGET:
        offset = _renderTargetDescriptorSize * _numRenderTarget;
        handle = _renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += offset;
        _numRenderTarget++;
        break;

    case ViewManager::Type::DEPTH_STENCIL:
        offset = _depthStencilDescriptorSize * _numDepthStencil;
        handle = _depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += offset;
        _numDepthStencil++;
        break;
    }
}

void ViewManager::AddDescriptorHeap(const ViewManager::Type type, DescriptorHandles& handle)
{
    UINT offset = 0;

    switch (type)
    {
    case ViewManager::Type::SHADER_RESOURCE:
        offset     = _shaderResourceDescriptorSize * _numShaderResource;
        handle.CPU = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart();
        handle.CPU.ptr += offset;
        handle.GPU = _shaderResourceHeap->GetGPUDescriptorHandleForHeapStart();
        handle.GPU.ptr += offset;
        _numShaderResource++;
        break;

    case ViewManager::Type::RENDER_TARGET:
        offset     = _renderTargetDescriptorSize * _numRenderTarget;
        handle.CPU = _renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
        handle.CPU.ptr += offset;
        _numRenderTarget++;
        break;

    case ViewManager::Type::DEPTH_STENCIL:
        offset     = _depthStencilDescriptorSize * _numDepthStencil;
        handle.CPU = _depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
        handle.CPU.ptr += offset;
        _numDepthStencil++;
        break;
    }
}
void ViewManager::AddDescriptorHeap(const ViewManager::Type type, UINT numDescriptors, std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>& handles)
{
    HRESULT hr     = S_OK;
    UINT    offset = 0;

    handles.resize(numDescriptors);

    for (UINT i = 0; i < numDescriptors; i++)
    {
        switch (type)
        {
        case ViewManager::Type::SHADER_RESOURCE:
            offset         = _shaderResourceDescriptorSize * _numShaderResource;
            handles[i] = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart();
            handles[i].ptr += offset;
            _numShaderResource++;
            break;

        case ViewManager::Type::RENDER_TARGET:
            offset         = _renderTargetDescriptorSize * _numRenderTarget;
            handles[i] = _renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
            handles[i].ptr += offset;
            _numRenderTarget++;
            break;

        case ViewManager::Type::DEPTH_STENCIL:
            offset         = _depthStencilDescriptorSize * _numDepthStencil;
            handles[i] = _depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
            handles[i].ptr += offset;
            _numDepthStencil++;
            break;
        }
    }
}

void ViewManager::AddDescriptorHeap(const ViewManager::Type type, UINT numDescriptors, std::vector<DescriptorHandles>& handles)
{
    HRESULT hr     = S_OK;
    UINT    offset = 0;

    handles.resize(numDescriptors);

    for (UINT i = 0; i < numDescriptors; i++)
    {
        switch (type)
        {
        case ViewManager::Type::SHADER_RESOURCE:
            offset         = _shaderResourceDescriptorSize * _numShaderResource;
            handles[i].CPU = _shaderResourceHeap->GetCPUDescriptorHandleForHeapStart();
            handles[i].CPU.ptr += offset;
            handles[i].GPU = _shaderResourceHeap->GetGPUDescriptorHandleForHeapStart();
            handles[i].GPU.ptr += offset;
            _numShaderResource++;
            break;

        case ViewManager::Type::RENDER_TARGET:
            offset         = _renderTargetDescriptorSize * _numRenderTarget;
            handles[i].CPU = _renderTargetHeap->GetCPUDescriptorHandleForHeapStart();
            handles[i].CPU.ptr += offset;
            _numRenderTarget++;
            break;

        case ViewManager::Type::DEPTH_STENCIL:
            offset         = _depthStencilDescriptorSize * _numDepthStencil;
            handles[i].CPU = _depthStencilHeap->GetCPUDescriptorHandleForHeapStart();
            handles[i].CPU.ptr += offset;
            _numDepthStencil++;
            break;
        }
    }
}