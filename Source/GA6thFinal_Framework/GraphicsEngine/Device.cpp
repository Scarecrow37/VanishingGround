#include "pch.h"
#include "Device.h"
#include "d3dUtil.h"

ComPtr<ID3D12Device5> Device::GetDevice5()
{
    ComPtr<ID3D12Device5> result;
    HRESULT               hr = _device->QueryInterface(IID_PPV_ARGS(result.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"DEvice::GetDevice5() Failed");
    return result;
}

ComPtr<ID3D12GraphicsCommandList4> Device::GetCommandList4()
{
    ComPtr<ID3D12GraphicsCommandList4> result;
    HRESULT                            hr = _commandList->QueryInterface(IID_PPV_ARGS(result.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"DEvice::GetDevice5() Failed");
    return result;
}

void Device::SetUpDevice(HWND hwnd, UINT width, UINT height, FeatureLevel feature)
{
    _mode.Width  = width;
    _mode.Height = height;
    _newMode     = _mode;
    _resolution  = {(LONG)width, (LONG)height};

#ifdef _DEBUG
    ComPtr<ID3D12Debug> debugController;

    HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
    FAILED_CHECK_MESSAGE(hr, L"Device::SetUpDevice D3D12GetDebugInterface Failed");
    debugController->EnableDebugLayer();
#endif

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
#ifdef _DEBUG
    ComPtr<ID3D12InfoQueue> infoQueue;
    if (SUCCEEDED(_device.As(&infoQueue)))
    {
        D3D12_MESSAGE_ID hideIDs[] = {D3D12_MESSAGE_ID_COMMAND_LIST_DRAW_VERTEX_BUFFER_TOO_SMALL,
                                      D3D12_MESSAGE_ID_CREATERESOURCE_STATE_IGNORED};

        D3D12_INFO_QUEUE_FILTER filter = {};
        filter.DenyList.NumIDs         = _countof(hideIDs);
        filter.DenyList.pIDList        = hideIDs;

        infoQueue->AddStorageFilterEntries(&filter);
        // 에러시 debug break
        //infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
    }
#endif
    if (Global::isRayTracing)
    {
        CheckDXRSupport();
    }
}

void Device::Initialize()
{
    _renderTargetHandles.resize(SWAPCHAIN_BUFFER_COUNT);

    for (auto& handle : _renderTargetHandles)
    {
        Global::viewManager->AddDescriptorHeap(ViewManager::Type::RENDER_TARGET, handle);
    }

    ResizeSwapChain();
}

void Device::Finalize()
{
    _uploadBuffers.clear();
}

void Device::OnResize(UINT width, UINT height)
{
    _newMode.Width  = width;
    _newMode.Height = height;
    _onResize       = true;
}

void Device::GPUSync()
{
    auto& commandController = Global::commandController;

    UINT64 fence = commandController->SignalCommandQueue(CommandQueueType::GRAPHICS_QUEUE);
    commandController->WaitForCommandQueue(CommandQueueType::GRAPHICS_QUEUE, fence);
}

void Device::FullGPUSync()
{
    auto& commandController = Global::commandController;

    UINT64 fence = commandController->SignalCommandQueue(CommandQueueType::GRAPHICS_QUEUE);
    commandController->WaitForCommandQueue(CommandQueueType::GRAPHICS_QUEUE, fence);
}

void Device::UploadResource(ComPtr<ID3D12Resource> uploadResource)
{
    _uploadBuffers.push_back(uploadResource);
}

void Device::ResolveBackBuffer(ComPtr<ID3D12Resource> source)
{
    _commandList->ResolveSubresource(_swapChainBuffer[_renderTargetIndex].Get(), 0, source.Get(), 0, _mode.Format);

    auto br =
        CD3DX12_RESOURCE_BARRIER::Transition(_swapChainBuffer[_renderTargetIndex].Get(),
                                             D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);

    _commandList->ResourceBarrier(1, &br);
}

void Device::ResetCommands()
{
    _commandAllocator->Reset();
    _commandList->Reset(_commandAllocator.Get(), nullptr);

    Global::commandController->ResetCommand(CommandQueueType::GRAPHICS_QUEUE);
}

void Device::ResetGraphicsCommnad()
{
    _commandList->Reset(_commandAllocator.Get(), nullptr);
}

void Device::ResetComputeCommands()
{
    _computeCommandAllocator->Reset();
    _computeCommandList->Reset(_computeCommandAllocator.Get(), nullptr);
}

void Device::Execute()
{
    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_swapChainBuffer[_renderTargetIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    _commandList->ResourceBarrier(1, &br);

    _computeCommandList->Close();
    _commandList->Close();

    auto& commandController = Global::commandController;

    // Execute Compute Commands
    commandController->ExecuteCommand(CommandQueueType::COMPUTE_QUEUE, _computeCommandList.Get());

    // Wait for Compute Commands to finish
    UINT64 computeFence = commandController->SignalCommandQueue(CommandQueueType::COMPUTE_QUEUE);
    commandController->WaitCommandQueue(CommandQueueType::GRAPHICS_QUEUE, CommandQueueType::COMPUTE_QUEUE, computeFence);

    // Draw Graphics Commands
    commandController->ExecuteCommand(CommandQueueType::GRAPHICS_QUEUE, _commandList.Get());
}

void Device::UpdateBuffer(ComPtr<ID3D12Resource>& buffer, const void* data, UINT64 size)
{
    HRESULT hr = S_OK;

    if (nullptr == data)
        return;

    void*                 temp = nullptr;
    constexpr D3D12_RANGE readRange{0, 0};
    hr = buffer->Map(0, &readRange, &temp);
    FAILED_CHECK_MESSAGE(hr, L"Device::UpdateBuffer buffer->Map Failed");

    memcpy(temp, data, size);

    D3D12_RANGE writeRange{0, size};
    buffer->Unmap(0, &writeRange);
}

void Device::ClearBackBuffer(UINT flag, XMVECTOR color, float depth, UINT stencil)
{
    D3D12_RECT rc = {0, 0, (LONG)_mode.Width, (LONG)_mode.Height};

    // 장치 상태 재설정.
    _commandList->RSSetViewports(1, &_mainViewport);
    _commandList->RSSetScissorRects(1, &rc);

    auto br = CD3DX12_RESOURCE_BARRIER::Transition(_swapChainBuffer[_renderTargetIndex].Get(),
                                                   D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    _commandList->ResourceBarrier(1, &br);
    _commandList->ClearRenderTargetView(_renderTargetHandles[_renderTargetIndex], (float*)&color, 0, nullptr);
}

void Device::Flip()
{
    ID3D12CommandQueue* commandQueue = Global::commandController->GetCommandQueue(CommandQueueType::GRAPHICS_QUEUE);
    _graphicsMemory->Commit(commandQueue);

    _swapChain->Present(0, 0);
    GPUSync();
    ResizeSwapChain();

    Global::particleManager->ChangeTexture();

    _uploadBuffers.clear();

    // 새 프레임 준비.
    _renderTargetIndex = _swapChain->GetCurrentBackBufferIndex();
}

void Device::CreateVertexBuffer(void* data, UINT size, UINT stride, ComPtr<ID3D12Resource>& buffer,
                                D3D12_VERTEX_BUFFER_VIEW& view)
{
    if (data)
    {
        ComPtr<ID3D12Resource> uploadBuffer;
        buffer = d3dUtil::CreateBufferWithData(_device.Get(), _commandList.Get(), data, size, uploadBuffer);

        _uploadBuffers.push_back(uploadBuffer);
    }

    view.BufferLocation = buffer->GetGPUVirtualAddress();
    view.SizeInBytes    = size;
    view.StrideInBytes  = stride;
}

void Device::CreateVertexBuffer(ID3D12GraphicsCommandList* commandList, void* data, UINT size, UINT stride,
                                ComPtr<ID3D12Resource>& buffer, D3D12_VERTEX_BUFFER_VIEW& view)
{
    if (data)
    {
        ComPtr<ID3D12Resource> uploadBuffer;
        buffer = d3dUtil::CreateBufferWithData(_device.Get(), commandList, data, size, uploadBuffer);

        std::unique_lock<std::mutex> lock(_uploadBufferMutex);
        _uploadBuffers.push_back(uploadBuffer);
    }

    view.BufferLocation = buffer->GetGPUVirtualAddress();
    view.SizeInBytes    = size;
    view.StrideInBytes  = stride;
}

void Device::CreateIndexBuffer(void* data, UINT size, DXGI_FORMAT format, ComPtr<ID3D12Resource>& buffer,
                               D3D12_INDEX_BUFFER_VIEW& view)
{
    if (data)
    {
        ComPtr<ID3D12Resource> uploadBuffer;
        buffer = d3dUtil::CreateBufferWithData(_device.Get(), _commandList.Get(), data, size, uploadBuffer);

        _uploadBuffers.push_back(uploadBuffer);
    }

    view.BufferLocation = buffer->GetGPUVirtualAddress();
    view.SizeInBytes    = size;
    view.Format         = format;
}

void Device::CreateIndexBuffer(ID3D12GraphicsCommandList* commandList, void* data, UINT size, DXGI_FORMAT format,
                               ComPtr<ID3D12Resource>& buffer, D3D12_INDEX_BUFFER_VIEW& view)
{
    if (data)
    {
        ComPtr<ID3D12Resource> uploadBuffer;
        buffer = d3dUtil::CreateBufferWithData(_device.Get(), commandList, data, size, uploadBuffer);

        std::unique_lock<std::mutex> lock(_uploadBufferMutex);
        _uploadBuffers.push_back(uploadBuffer);
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
    hr         = _device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_COMMON, nullptr,
                                                  IID_PPV_ARGS(&buffer));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDefaultBuffer _device->CreateCommittedResource Failed");
}

void Device::CreateDefaultBuffer(UINT size, const D3D12_RESOURCE_FLAGS flags, const D3D12_RESOURCE_STATES initState,
                                 ComPtr<ID3D12Resource>& buffer)
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
    rd.Flags               = flags;

    // 버퍼 생성.
    ID3D12Resource* pBuff = nullptr;

    HRESULT hr = S_OK;
    hr = _device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, initState, nullptr, IID_PPV_ARGS(&buffer));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDefaultBuffer _device->CreateCommittedResource Failed");
}

void Device::CreateUploadBuffer(UINT size, const D3D12_RESOURCE_FLAGS flags, const D3D12_RESOURCE_STATES initState,
                                ComPtr<ID3D12Resource>& buffer)
{
    D3D12_HEAP_PROPERTIES hp = {};
    hp.Type                  = D3D12_HEAP_TYPE_UPLOAD;
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
    rd.Flags               = flags;

    // 버퍼 생성.
    ID3D12Resource* pBuff = nullptr;

    HRESULT hr = S_OK;
    hr = _device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, initState, nullptr, IID_PPV_ARGS(&buffer));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateUploadBuffer _device->CreateCommittedResource Failed");
}

void Device::CreateCommandList(ComPtr<ID3D12CommandAllocator>&    allocator,
                               ComPtr<ID3D12GraphicsCommandList>& commandList, CommandType type)
{
    D3D12_COMMAND_QUEUE_DESC desc{
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
    hr         = _device->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(&allocator));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateCommandList _device->CreateCommandAllocator Failed");

    hr = _device->CreateCommandList(desc.NodeMask, desc.Type, allocator.Get(), nullptr, IID_PPV_ARGS(&commandList));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateCommandList _device->CreateCommandList Failed");

    commandList->Close();
}

ComPtr<ID3D12RootSignature> Device::CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc)
{
    ComPtr<ID3D10Blob> pSigBlob;
    ComPtr<ID3D10Blob> pErrorBlob;

    HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &pSigBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        std::string msg(static_cast<const char*>(pErrorBlob->GetBufferPointer()), pErrorBlob->GetBufferSize());
        OutputDebugStringA(msg.c_str());
        return nullptr;
    }

    ComPtr<ID3D12RootSignature> pRootSig;
    hr = S_OK;
    hr = _device->CreateRootSignature(0, pSigBlob->GetBufferPointer(), pSigBlob->GetBufferSize(),
                                      IID_PPV_ARGS(pRootSig.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateRootSignature _device->CreateRootSignature Failed");

    return pRootSig;
}

void Device::ResizeSwapChain()
{
    if (!_onResize)
        return;

    GRAPHICS_ASSERT(_device || _swapChain, L"");

    _commandList->Reset(_commandAllocator.Get(), nullptr);

    CreateBackBuffer();

    _commandList->Close();
    Global::commandController->ExecuteCommand(CommandQueueType::GRAPHICS_QUEUE, _commandList.Get());

    FullGPUSync();

    _mainViewport.TopLeftX = 0;
    _mainViewport.TopLeftY = 0;
    _mainViewport.Width    = static_cast<float>(_newMode.Width);
    _mainViewport.Height   = static_cast<float>(_newMode.Height);
    _mainViewport.MinDepth = 0.0f;
    _mainViewport.MaxDepth = 1.0f;

    _mainScissorRect = {0, 0, static_cast<long>(_newMode.Width), static_cast<long>(_newMode.Height)};

    DXGI_MODE_DESC prevMode = _mode;
    _mode                   = _newMode;

    // Global::dxResourceManager->ResizeResource(prevMode);
    // _size = {_newMode.Width, _newMode.Height};

    _onResize = false;
}

void Device::CreateDeviceAndSwapChain(HWND hwnd, D3D_FEATURE_LEVEL feature)
{
    HRESULT hr = S_OK;

    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&_dxgiFactory));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDeviceAndSwapChain CreateDXGIFactory2 Failed");

    hr = D3D12CreateDevice(0, feature, IID_PPV_ARGS(&_device));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDeviceAndSwapChain D3D12CreateDevice Failed");

    Global::commandController->Initialize();

    _rtvDescriptorSize       = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    _dsvDescriptorSize       = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    _cbvSrvUavDescriptorSize = _device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // 4X MSAA 품질 수준 지원 점검
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Flags            = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.SampleCount      = 4;
    msQualityLevels.Format           = _backBufferFormat;
    msQualityLevels.NumQualityLevels = 0;

    hr = _device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels,
                                      sizeof(msQualityLevels));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDeviceAndSwapChain _device->CheckFeatureSupport Failed");

    _4xMSAAQuality = msQualityLevels.NumQualityLevels;
    GRAPHICS_ASSERT(_4xMSAAQuality > 0, L"Unexpected MSAA quality level");

    //_commanSet = Global::commandController->CreateCommandSet(CommandType::DIRECT, CommandQueueType::GRAPHICS_QUEUE,
    //L"GraphicsCommandS");

    CreateCommandList(_commandAllocator, _commandList, CommandType::DIRECT);
    CreateCommandList(_computeCommandAllocator, _computeCommandList, CommandType::COMPUTE);

    _commandList->SetName(L"GraphicsCmdList");
    _commandAllocator->SetName(L"GraphicsCmdAllocator");

    _commandList->SetName(L"GraphicsCmdList");
    _computeCommandList->SetName(L"ComputeCmdList");

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

    ID3D12CommandQueue* commandQueue = Global::commandController->GetCommandQueue(CommandQueueType::GRAPHICS_QUEUE);

    ComPtr<IDXGISwapChain1> swapChain;
    hr = _dxgiFactory->CreateSwapChainForHwnd(commandQueue, hwnd, &sd, nullptr, nullptr, &swapChain);
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDeviceAndSwapChain _dxgiFactory->CreateSwapChainForHwnd Failed");

    hr = swapChain->QueryInterface(IID_PPV_ARGS(&_swapChain));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateDeviceAndSwapChain swapChain->QueryInterface Failed");
    _renderTargetIndex = _swapChain->GetCurrentBackBufferIndex();

    _graphicsMemory = std::make_unique<GraphicsMemory>(_device.Get());
    //_graphicsMemory->Allocate()
}

void Device::CreateBackBuffer()
{
    _newMode.Format = _backBufferFormat;
    for (auto& swapChain : _swapChainBuffer)
    {
        swapChain.Reset();
    }

    HRESULT hr = S_OK;
    hr         = _swapChain->ResizeBuffers(SWAPCHAIN_BUFFER_COUNT, _newMode.Width, _newMode.Height, _newMode.Format,
                                           DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
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
    hr = _device->CreateCommittedResource(&hp, D3D12_HEAP_FLAG_NONE, &rd, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                          IID_PPV_ARGS(&buffer));
    FAILED_CHECK_MESSAGE(hr, L"Device::CreateBuffer _device->CreateCommittedResource Failed");
}

void Device::CheckDXRSupport()
{
    D3D12_FEATURE_DATA_D3D12_OPTIONS5 options5{};
    HRESULT                           hr = S_OK;
    hr = _device->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS5, &options5, sizeof(options5));
    FAILED_CHECK_MESSAGE(hr, L"Device::CheckDXRSupport _device->CheckFeatureSupport Failed");

    if (options5.RaytracingTier < D3D12_RAYTRACING_TIER_1_0)
    {
        hr = E_FAIL;
    }
    FAILED_CHECK_MESSAGE(hr, L"This Device not supported ray tracing");
}