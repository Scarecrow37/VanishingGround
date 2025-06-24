#include "pch.h"
#include "Device.h"
#include "d3dUtil.h"

//void Device::SignalComputeQueue(int fenceSlot)
//{
//    const UINT64 fenceValue = _fenceValues[fenceSlot]++;
//    _computeCommandQueue->Signal(_graphicsFences[fenceSlot].Get(), fenceValue);
//    _lastGraphicsFenceValues[fenceSlot] = fenceValue;
//}
//void Device::SignalGraphicsQueue(int fenceSlot)
//{
//    const UINT64 fenceValue = _fenceValues[fenceSlot]++;
//    _commandQueue->Signal(_graphicsFences[fenceSlot].Get(), fenceValue);
//    _lastGraphicsFenceValues[fenceSlot] = fenceValue;
//}

void Device::SetUpDevice(HWND hwnd, UINT width, UINT height, FeatureLevel feature)
{
    _mode.Width  = width;
    _mode.Height = height;
#ifndef NDEBUG
    ComPtr<ID3D12Debug> debugController;

    HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
    FAILED_CHECK_MESSAGE(hr, L"Device::SetUpDevice D3D12GetDebugInterface Failed");
    debugController->EnableDebugLayer();
#endif // !NDEBUG
    D3D_FEATURE_LEVEL d3dFeature{};

    switch (feature)
    {
    case FeatureLevel::LEVEL_11_0:
        d3dFeature = D3D_FEATURE_LEVEL_11_0;
        break;
    case FeatureLevel::LEVEL_12_0:
        d3dFeature = D3D_FEATURE_LEVEL_12_0;
        break;
    case FeatureLevel::LEVEL_12_1:
        d3dFeature = D3D_FEATURE_LEVEL_12_1;
        break;
    default:
        break;
    }

    CreateDeviceAndSwapChain(hwnd, d3dFeature);
}

void Device::Initialize()
{
    _commandLists.resize(COMMAND_LIST_END);

    UmViewManager.AddDescriptorHeap(ViewManager::Type::RENDER_TARGET, SWAPCHAIN_BUFFER_COUNT, _renderTargetHandles);

    ResizeSwapChain();
}

void Device::Finalize()
{
    _uploadResources.clear();
    _graphicsFences.clear();
    _lastGraphicsFenceValues.clear();
    CloseHandle(_fenceEvent);
}

void Device::OnResize(UINT width, UINT height)
{
    _mode.Width  = width;
    _mode.Height = height;
    _onResize    = true;
}

void Device::GPUSync()
{
    const UINT64 fence = _fenceValue;

    // Fence 값 갱신
    _commandQueue->Signal(_fence.Get(), fence);

    _fenceValue++;

    // GPU 의 현재 Fence 값 확인.
    if (_fence->GetCompletedValue() < fence)
    {
        // 이벤트 설정 : GPU 의 펜스 값이 fence 와 동일해지면 이벤트가 발생됨.
        _fence->SetEventOnCompletion(fence, _fenceEvent);

        // 대기...
        ::WaitForSingleObject(_fenceEvent, INFINITE);
    }
}

void Device::UploadResource(ComPtr<ID3D12Resource> uploadResource) 
{
    _uploadResources.push_back(uploadResource);
}

void Device::SetBackBuffer()
{
    _commandList->OMSetRenderTargets(1, &_renderTargetHandles[_renderTargetIndex], FALSE, nullptr);
}

void Device::ResolveBackBuffer(ComPtr<ID3D12Resource> source)
{
    _commandList->ResolveSubresource(_swapChainBuffer[_renderTargetIndex].Get(), 0, source.Get(), 0, _mode.Format);

    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_swapChainBuffer[_renderTargetIndex].Get(), D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);

    _commandList->ResourceBarrier(1, &br);
}

void Device::ResetCommands()
{
    _commandAllocator->Reset();
    _commandList->Reset(_commandAllocator.Get(), nullptr);
    _imguiCommandAllocator->Reset();
    _imguiCommandList->Reset(_imguiCommandAllocator.Get(), nullptr);
    _postProcessCommandAllocator->Reset();
    _postProcessCommandList->Reset(_postProcessCommandAllocator.Get(), nullptr);
}

void Device::ResetComputeCommands()
{
    _computeCommandAllocator->Reset();
    _computeCommandList->Reset(_computeCommandAllocator.Get(), nullptr);
}

void Device::Execute()
{
    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_swapChainBuffer[_renderTargetIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    _imguiCommandList->ResourceBarrier(1, &br);

    _computeCommandList->Close();
    _commandList->Close();
    _imguiCommandList->Close();
    _postProcessCommandList->Close();

    RegisterCommand(_computeCommandList.Get(), MESH_COMPUTE_LIST);
    RegisterCommand(_commandList.Get(), MESH_RENDER_LIST);
    RegisterCommand(_imguiCommandList.Get(),IMGUI_RENDER_LIST);
    RegisterCommand(_postProcessCommandList.Get(), POST_PROCESS_LIST);

    // [1] 메시 컴퓨트 작업 (Compute Queue)
    ExecuteCommand(MESH_COMPUTE_LIST);
    SignalComputeQueue(MESH_COMPUTE_FENCE);

    // [2] 컴퓨트 큐 작업 완료 대기 (Graphics Queue)
    _commandQueue->Wait(_graphicsFences[MESH_COMPUTE_FENCE].Get(), _lastGraphicsFenceValues[MESH_COMPUTE_FENCE]);

    // [3] 병렬 실행: 파티클 컴퓨트 + 메시 렌더
    //--------------------------------------------------
    // (A) 파티클 컴퓨트 작업 (Compute Queue)
    ExecuteCommand(PARTICLE_COMPUTE_LIST);
    SignalComputeQueue(PARTICLE_COMPUTE_FENCE);
    // (B) 메시 렌더 작업 (Graphics Queue)
    ExecuteCommand(MESH_RENDER_LIST);
    SignalGraphicsQueue(MESH_RENDER_FENCE);
    //--------------------------------------------------

    // [4] 파티클 렌더 전 동기화
    // 컴퓨트 큐 + 그래픽 큐 작업 모두 완료 대기
    _commandQueue->Wait(_graphicsFences[PARTICLE_COMPUTE_FENCE].Get(), _lastGraphicsFenceValues[PARTICLE_COMPUTE_FENCE]);
    _commandQueue->Wait(_graphicsFences[MESH_RENDER_FENCE].Get(), _lastGraphicsFenceValues[MESH_RENDER_FENCE]);

    // [5] 파티클 렌더 실행 (Graphics Queue)
    ExecuteCommand(PARTICLE_RENDER_LIST);
    SignalGraphicsQueue(PARTICLE_RENDER_FENCE);
    
    // [6] 포트스 프로세싱 렌더 실행 (Graphics Queue)
    ExecuteCommand(POST_PROCESS_LIST);
    SignalGraphicsQueue(POST_PROCESS_FENCE);

    // [7] 임구이 렌더 전 동기화
    // 그래픽 큐 작업 완료 대기
    _commandQueue->Wait(_graphicsFences[PARTICLE_RENDER_FENCE].Get(), _lastGraphicsFenceValues[PARTICLE_RENDER_FENCE]);
    // [8] 임구이 렌더 실행 (Graphics Queue)
    ExecuteCommand(IMGUI_RENDER_LIST);
}

void Device::UpdateBuffer(ComPtr<ID3D12Resource>& buffer, void* data, UINT size)
{
    HRESULT hr = S_OK;

    if (nullptr == data)
        return;

    UINT8* temp = nullptr;
    hr          = buffer->Map(0, nullptr, (void**)&temp);
    FAILED_CHECK_MESSAGE(hr, L"Device::UpdateBuffer buffer->Map Failed");

    memcpy(temp, data, size);
    buffer->Unmap(0, nullptr);
}

void Device::ClearBackBuffer(UINT flag, XMVECTOR color, float depth, UINT stencil)
{
    D3D12_RECT rc = {0, 0, (LONG)_mode.Width, (LONG)_mode.Height};

    // 커맨드 리셋.
    /*_commandAllocator->Reset();
    _commandList->Reset(_commandAllocator.Get(), _currentPipelineState.Get());*/

    // 장치 상태 재설정.
    _commandList->RSSetViewports(1, &_mainViewport);
    _commandList->RSSetScissorRects(1, &rc);
    _imguiCommandList->RSSetViewports(1, &_mainViewport);
    _imguiCommandList->RSSetScissorRects(1, &rc);
    // 렌더타겟 상태 전환
    //<리소스 베리어> 각 리소스의 상태관리 인터페이스. 리소스의 운용 충돌(Resource Hazard) 방지용.

    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_swapChainBuffer[_renderTargetIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    _commandList->ResourceBarrier(1, &br);

    _imguiCommandList->OMSetRenderTargets(1, &_renderTargetHandles[_renderTargetIndex], FALSE, nullptr);
    _commandList->ClearRenderTargetView(_renderTargetHandles[_renderTargetIndex], (float*)&color, 0, nullptr);
}

void Device::Flip()
{
    _swapChain->Present(0, 0);
    GPUSync();
    ResizeSwapChain();

    _uploadResources.clear();

    // 새 프레임 준비.
    _renderTargetIndex = _swapChain->GetCurrentBackBufferIndex();
}
  
void Device::CreateVertexBuffer(void* data, UINT size, UINT stride, ComPtr<ID3D12Resource>& buffer, D3D12_VERTEX_BUFFER_VIEW& view)
{
    if (data)
    {
        ComPtr<ID3D12Resource> uploadBuffer;
        buffer = d3dUtil::CreateDefaultBuffer(_device.Get(), _commandList.Get(), data, size, uploadBuffer);

        _uploadResources.push_back(uploadBuffer);
    }

    view.BufferLocation = buffer->GetGPUVirtualAddress();
    view.SizeInBytes    = size;
    view.StrideInBytes  = stride;
}

void Device::CreateIndexBuffer(void* data, UINT size, DXGI_FORMAT format, ComPtr<ID3D12Resource>& buffer, D3D12_INDEX_BUFFER_VIEW& view)
{
    if (data)
    {
        ComPtr<ID3D12Resource> uploadBuffer;
        buffer = d3dUtil::CreateDefaultBuffer(_device.Get(), _commandList.Get(), data, size, uploadBuffer);

        _uploadResources.push_back(uploadBuffer);
    }

    view.BufferLocation = buffer->GetGPUVirtualAddress();
    view.SizeInBytes    = size;
    view.Format         = format;
}

void Device::CreateConstantBuffer(void* data, UINT size, ComPtr<ID3D12Resource>& buffer)
{
    // 버퍼 생성
    CreateBuffer(size, buffer);

    // 버퍼 갱신
    if (data)
        UpdateBuffer(buffer, data, size);
}

void Device::CreateDefaultBuffer(UINT size, ComPtr<ID3D12Resource>& buffer)
{
    D3D12_HEAP_PROPERTIES hp = {};
    hp.Type                  = D3D12_HEAP_TYPE_DEFAULT;
    hp.CPUPageProperty       = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    hp.MemoryPoolPreference  = D3D12_MEMORY_POOL_UNKNOWN;
    hp.CreationNodeMask      = 0;
    hp.VisibleNodeMask       = 0;

    // 버퍼에 저장될 자원 정보 설정
    D3D12_RESOURCE_DESC rd = {};
    rd.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER; // 자원 형식 : "버퍼"
    rd.Alignment           = 0;    // 기본 정렬 64KB (D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT)
    rd.Width               = size; // 저장할 자원(데이터)의 크기.
    rd.Height              = 1;
    rd.DepthOrArraySize    = 1;
    rd.MipLevels           = 1;
    rd.Format              = DXGI_FORMAT_UNKNOWN;
    rd.SampleDesc.Count    = 1;
    rd.SampleDesc.Quality  = 0;
    rd.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    rd.Flags               = D3D12_RESOURCE_FLAG_NONE;

    // 버퍼 생성.
    ID3D12Resource* pBuff = nullptr;

    HRESULT hr = S_OK;
    hr = _device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDefaultBuffer _device->CreateCommittedResource Failed");
}

void Device::CreateCommandList(ComPtr<ID3D12CommandAllocator>& allocator, ComPtr<ID3D12GraphicsCommandList>& commandList, CommandType type)
{
    D3D12_COMMAND_QUEUE_DESC desc
    {
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
    };

    switch (type)
    {
    case CommandType::DIRECT:
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        break;
    case CommandType::BUNDLE:
        desc.Type = D3D12_COMMAND_LIST_TYPE_BUNDLE;
        break;
    case CommandType::COMPUTE:
        desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
        break;
    }

    HRESULT hr = S_OK;
    hr = _device->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(&allocator));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateCommandList _device->CreateCommandAllocator Failed");

    hr = _device->CreateCommandList(desc.NodeMask, desc.Type, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateCommandList _device->CreateCommandList Failed");

    commandList->Close();
}

void Device::RegisterCommand(ID3D12CommandList* commandList, CommandListType type)
{
    _commandLists[type].push_back(commandList);
}

void Device::ExecuteCommand(CommandListType type)
{
    switch (type)
    {
    case CommandListType::MESH_RENDER_LIST:
    case CommandListType::PARTICLE_RENDER_LIST:
        _commandQueue->ExecuteCommandLists(static_cast<UINT>(_commandLists[type].size()), _commandLists[type].data());
        break;
    case CommandListType::MESH_COMPUTE_LIST:
    case CommandListType::PARTICLE_COMPUTE_LIST:
        _computeCommandQueue->ExecuteCommandLists(static_cast<UINT>(_commandLists[type].size()), _commandLists[type].data());
        break;
    case CommandListType::IMGUI_RENDER_LIST:
        _commandQueue->ExecuteCommandLists(static_cast<UINT>(_commandLists[type].size()), _commandLists[type].data());
        break;
    case CommandListType::POST_PROCESS_LIST:
        _commandQueue->ExecuteCommandLists(static_cast<UINT>(_commandLists[type].size()), _commandLists[type].data());
        break;
    }

    _commandLists[type].clear();
}

void Device::SetViewPort()
{
    D3D12_VIEWPORT viewPort{
        .TopLeftX = 0,
        .TopLeftY = 0,
        .Width    = (FLOAT)_mode.Width,
        .Height   = (FLOAT)_mode.Height,
        .MinDepth = 0.0f,
        .MaxDepth = 1.0f,
    };

    // g_CmdList->RSSetViewports(1, &vp);
    _mainViewport = viewPort;
}

void Device::ResizeSwapChain()
{
    if (!_onResize)
        return;

    GRAPHICS_ASSERT(_device || _swapChain, L"");

    GPUSync();
    _commandList->Reset(_commandAllocator.Get(), nullptr);

    CreateBackBuffer();

    _commandList->Close();

    RegisterCommand(_commandList.Get(), MESH_RENDER_LIST);
    ExecuteCommand(MESH_RENDER_LIST);
    GPUSync();

    _mainViewport.TopLeftX = 0;
    _mainViewport.TopLeftY = 0;
    _mainViewport.Width    = static_cast<float>(_mode.Width);
    _mainViewport.Height   = static_cast<float>(_mode.Height);
    _mainViewport.MinDepth = 0.0f;
    _mainViewport.MaxDepth = 1.0f;

    _mainrRect = {0, 0, static_cast<long>(_mode.Width), static_cast<long>(_mode.Height)};
    _onResize  = false;
}

void Device::CreateDeviceAndSwapChain(HWND hwnd, D3D_FEATURE_LEVEL feature)
{
    HRESULT hr = S_OK;

    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&_dxgiFactory));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDeviceAndSwapChain CreateDXGIFactory2 Failed");

    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&_dxgiFactory));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDeviceAndSwapChain CreateDXGIFactory2 Failed");

    hr = D3D12CreateDevice(0, feature, IID_PPV_ARGS(&_device));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDeviceAndSwapChain D3D12CreateDevice Failed");

    _rtvDescriptorSize       = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    _dsvDescriptorSize       = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    _cbvSrvUavDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // 4X MSAA 품질 수준 지원 점검
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Flags            = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.SampleCount      = 4;
    msQualityLevels.Format           = _backBufferFormat;
    msQualityLevels.NumQualityLevels = 0;

    hr = _device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDeviceAndSwapChain _device->CheckFeatureSupport Failed");

    _4xMSAAQuality = msQualityLevels.NumQualityLevels;
    GRAPHICS_ASSERT(_4xMSAAQuality > 0, L"Unexpected MSAA quality level");

    CreateCommandQueue();
    CreateComputeCommandObject();
    CreateSyncObject();

    _swapChain.Reset();
    DXGI_SWAP_CHAIN_DESC1 sd{};

    sd.Width              = _mode.Width;
    sd.Height             = _mode.Height;
    sd.Format             = _backBufferFormat;
    sd.SampleDesc.Count   = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 후면 버퍼의 속성
    sd.BufferCount        = SWAPCHAIN_BUFFER_COUNT;
    sd.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    ComPtr<IDXGISwapChain1> swapChain;
    hr = _dxgiFactory->CreateSwapChainForHwnd(_commandQueue.Get(), hwnd, &sd, nullptr, nullptr, &swapChain);
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDeviceAndSwapChain _dxgiFactory->CreateSwapChainForHwnd Failed");

    hr = swapChain->QueryInterface(IID_PPV_ARGS(&_swapChain));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDeviceAndSwapChain swapChain->QueryInterface Failed");
    _renderTargetIndex = _swapChain->GetCurrentBackBufferIndex();
}

void Device::CreateComputeCommandObject()
{
    CreateCommandList(_computeCommandAllocator, _computeCommandList, CommandType::COMPUTE);
    D3D12_COMMAND_QUEUE_DESC desc
    {
        .Type     = D3D12_COMMAND_LIST_TYPE_COMPUTE,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
    };

    HRESULT hr = S_OK;
    hr         = _device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_computeCommandQueue));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateComputeCommandObject _device->CreateCommandQueue Failed");
}

void Device::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc
    {
        .Type     = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
    };

    HRESULT hr = S_OK;

    hr = _device->CreateCommandQueue(&desc, IID_PPV_ARGS(&_commandQueue));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateCommandQueue _device->CreateCommandQueue Failed");

    CreateCommandList(_commandAllocator, _commandList, CommandType::DIRECT);
    CreateCommandList(_imguiCommandAllocator, _imguiCommandList, CommandType::DIRECT);
    CreateCommandList(_postProcessCommandAllocator, _postProcessCommandList, CommandType::DIRECT);

    _commandList->Close();
    _imguiCommandList->Close();
    _postProcessCommandList->Close();

    _commandQueue->SetName(L"GraphicsQueue");
    _commandList->SetName(L"GraphicsCmdList");
    _imguiCommandList->SetName(L"imguiCmdList");
    _postProcessCommandList->SetName(L"PostProcessCmdList");
}

void Device::CreateSyncObject()
{
    HRESULT hr = S_OK;
    hr         = _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_fence));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateSyncObject _device->CreateFence Failed");

    _fenceValue = 1;
    _fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    GRAPHICS_ASSERT(nullptr != _fenceEvent, L"Device::CreateSyncObject CreateEvent Failed");

    _graphicsFences.resize(FENCE_END);
    _lastGraphicsFenceValues.resize(FENCE_END);
    _fenceValues.resize(FENCE_END);

    for (UINT i = MESH_COMPUTE_FENCE; i < FENCE_END; ++i)
    {
        hr = _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_graphicsFences[i]));
        FAILED_CHECK_MESSAGE(hr, L"Device::CreateSyncObject _device->CreateFence Failed");
        _fenceValues[i] = 1;
    }
}

void Device::CreateBackBuffer()
{
    _mode.Format = _backBufferFormat;
    for (auto & swapChain : _swapChainBuffer)
    {
        swapChain.Reset();
    }

    HRESULT hr = S_OK;
    hr         = _swapChain->ResizeBuffers(SWAPCHAIN_BUFFER_COUNT, _mode.Width, _mode.Height, _mode.Format, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateBackBuffer _swapChain->ResizeBuffers Failed");

    _renderTargetIndex = _swapChain->GetCurrentBackBufferIndex();
    for (UINT i = 0; i < SWAPCHAIN_BUFFER_COUNT; i++)
    {
        hr = _swapChain->GetBuffer(i, IID_PPV_ARGS(&_swapChainBuffer[i]));
        FAILED_CHECK_MESSAGE(hr, L"Device::CreateBackBuffer _swapChain->GetBuffer Failed");
        _device->CreateRenderTargetView(_swapChainBuffer[i].Get(), nullptr, _renderTargetHandles[i]);
    }
}

void Device::CreateBuffer(UINT size, ComPtr<ID3D12Resource>& buffer)
{
    D3D12_HEAP_PROPERTIES hp = {};
    hp.Type                  = D3D12_HEAP_TYPE_UPLOAD; // 힙 타입 : "업로드"
    hp.CPUPageProperty       = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    hp.MemoryPoolPreference  = D3D12_MEMORY_POOL_UNKNOWN;
    hp.CreationNodeMask      = 0;
    hp.VisibleNodeMask       = 0;

    // 버퍼에 저장될 자원 정보 설정
    D3D12_RESOURCE_DESC rd = {};
    rd.Dimension           = D3D12_RESOURCE_DIMENSION_BUFFER; // 자원 형식 : "버퍼"
    rd.Alignment           = 0;    // 기본 정렬 64KB (D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT)
    rd.Width               = size; // 저장할 자원(데이터)의 크기.
    rd.Height              = 1;
    rd.DepthOrArraySize    = 1;
    rd.MipLevels           = 1;
    rd.Format              = DXGI_FORMAT_UNKNOWN;
    rd.SampleDesc.Count    = 1;
    rd.SampleDesc.Quality  = 0;
    rd.Layout              = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    rd.Flags               = D3D12_RESOURCE_FLAG_NONE;

    // 버퍼 생성.
    ID3D12Resource* pBuff = nullptr;

    HRESULT hr = S_OK;
    hr = _device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateBuffer _device->CreateCommittedResource Failed");
}

void Device::WaitComputeFence(int fenceSlot)
{
    UINT64 fenceValue = _lastGraphicsFenceValues[fenceSlot];
    _computeCommandQueue->Wait(_graphicsFences[fenceSlot].Get(), fenceValue);

    //if (_graphicsFences[fenceSlot]->GetCompletedValue() < fenceValue)
    //{
    //    _graphicsFences[fenceSlot]->SetEventOnCompletion(fenceValue, _fenceEvent);
    //    WaitForSingleObject(_fenceEvent, INFINITE);
    //}
}


void Device::WaitGraphicsFence(int fenceSlot)
{
    UINT64 fenceValue = _lastGraphicsFenceValues[fenceSlot];
    _commandQueue->Wait(_graphicsFences[fenceSlot].Get(), fenceValue);
    //if (_graphicsFences[fenceSlot]->GetCompletedValue() < fenceValue)
    //{
    //    _graphicsFences[fenceSlot]->SetEventOnCompletion(fenceValue, _fenceEvent);
    //    WaitForSingleObject(_fenceEvent, INFINITE);
    //}
}

void Device::SignalComputeQueue(int fenceSlot)
{
    _lastGraphicsFenceValues[fenceSlot]++;
    _computeCommandQueue->Signal(_graphicsFences[fenceSlot].Get(), _lastGraphicsFenceValues[fenceSlot]);
}

void Device::SignalGraphicsQueue(int fenceSlot)
{
    _lastGraphicsFenceValues[fenceSlot]++;
    _commandQueue->Signal(_graphicsFences[fenceSlot].Get(), _lastGraphicsFenceValues[fenceSlot]);
}