#pragma once

class Device
{
public:
    Device()  = default;
    ~Device() = default;

public:
    ID3D12Device*              GetDevice() const { return _device.Get(); }
    ComPtr<ID3D12Device5>      GetDevice5();
    ID3D12CommandQueue*        GetCommandQueue() const { return _commandQueue.Get(); }
    ID3D12CommandQueue*        GetComputeCommandQueue() const { return _computeCommandQueue.Get(); }
    ID3D12GraphicsCommandList* GetCommandList() const { return _commandList.Get(); }

    ComPtr<ID3D12GraphicsCommandList4>      GetCommandList4();
    ID3D12GraphicsCommandList*  GetImguiCommandList() const { return _imguiCommandList.Get(); }
    ID3D12GraphicsCommandList*  GetComputeCommandList() const { return _computeCommandList.Get(); }
    ID3D12GraphicsCommandList*  GetPostProcessCommandList() const { return _postProcessCommandList.Get(); }
    const DXGI_MODE_DESC&       GetMode() const { return _mode; }
    UINT                        GetRTVDescriptorSize() { return _rtvDescriptorSize; }
    UINT                        GetCBVSRVUAVDescriptorSize() { return _cbvSrvUavDescriptorSize; }
    UINT                        GetDSVDescriptorSize() { return _dsvDescriptorSize; }
    DXGI_FORMAT                 GetBackBufferFormat() { return _backBufferFormat; }
    bool                        GetMSAAState() { return _4xMSAAState; }
    UINT                        GetMSAAQuality() { return _4xMSAAQuality; }
    UINT                        GetCurrentBackBufferIndex() { return _renderTargetIndex; }
    D3D12_VIEWPORT              GetMainViewport() { return _mainViewport; }


public:
    void SetUpDevice(HWND hwnd, UINT width, UINT height, FeatureLevel feature);
    void Initialize();
    void Finalize();

public:
    void OnResize(UINT width, UINT height);
    void GPUSync();

    void UploadResource(ComPtr<ID3D12Resource> uploadResource);
    void SetBackBuffer();
    void ResolveBackBuffer(ComPtr<ID3D12Resource> source);

    void ResetCommands();
    void ResetComputeCommands();

    void Execute();

public:
    void UpdateBuffer(ComPtr<ID3D12Resource>& buffer, void* data, UINT size);

    void ClearBackBuffer(UINT flag, XMVECTOR color, float depth = 1.0f, UINT stencil = 0);
    void Flip();

    void CreateVertexBuffer(void* data, UINT size, UINT stride, ComPtr<ID3D12Resource>& buffer, D3D12_VERTEX_BUFFER_VIEW& view);
    void CreateIndexBuffer(void* data, UINT size, DXGI_FORMAT format, ComPtr<ID3D12Resource>& buffer, D3D12_INDEX_BUFFER_VIEW& view);
    void CreateConstantBuffer(void* data, UINT size, ComPtr<ID3D12Resource>& buffer);
    void CreateDefaultBuffer(UINT size, ComPtr<ID3D12Resource>& buffer);
    void CreateCommandList(ComPtr<ID3D12CommandAllocator>& allocator, ComPtr<ID3D12GraphicsCommandList>& commandList, CommandType type);
    void CreateDefaultBuffer(UINT size, const D3D12_RESOURCE_FLAGS flags, const D3D12_RESOURCE_STATES initState,
	ComPtr<ID3D12Resource>& buffer);
    void CreateUploadBuffer(UINT size, const D3D12_RESOURCE_FLAGS flags, const D3D12_RESOURCE_STATES initState,
	ComPtr<ID3D12Resource>& buffer);
    ComPtr<ID3D12RootSignature> CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc);
    void RegisterCommand(ID3D12CommandList* commandList, CommandListType type);
    void ExecuteCommand(CommandListType type);

private:
    void SetViewPort();
    void ResizeSwapChain();
    void CreateDeviceAndSwapChain(HWND hwnd, D3D_FEATURE_LEVEL feature);
    void CreateComputeCommandObject();
    void CreateCommandQueue();
    void CreateSyncObject();
    void CreateBackBuffer();
    void CreateBuffer(UINT size, ComPtr<ID3D12Resource>& buffer);

    void WaitComputeFence(int fenceSlot);
    void WaitGraphicsFence(int fenceSlot);
    void SignalComputeQueue(int fenceSlot);
    void SignalGraphicsQueue(int fenceSlot);

private:
    std::unique_ptr<GraphicsMemory> _graphicsMemory;
    ComPtr<ID3D12Device>       _device;
    ComPtr<IDXGIFactory4>      _dxgiFactory;
    ComPtr<IDXGISwapChain4>    _swapChain;
    ComPtr<ID3D12CommandQueue> _commandQueue;
    ComPtr<ID3D12Fence>        _fence;
    UINT64                     _fenceValue = 0;
    HANDLE                     _fenceEvent;

    std::vector<ComPtr<ID3D12Fence>> _graphicsFences;
    std::vector<UINT64>              _lastGraphicsFenceValues;
    std::vector<UINT64>              _fenceValues;

    ComPtr<ID3D12Resource>                   _swapChainBuffer[SWAPCHAIN_BUFFER_COUNT];
    UINT                                     _renderTargetIndex;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> _renderTargetHandles;

    D3D12_VIEWPORT _mainViewport;
    D3D12_RECT     _mainrRect;

    bool _4xMSAAState = false; // 4X MSAA enabled

    UINT _4xMSAAQuality           = 0; // quality level of 4X MSAA
    UINT _rtvDescriptorSize       = 0;
    UINT _dsvDescriptorSize       = 0;
    UINT _cbvSrvUavDescriptorSize = 0;

    D3D_DRIVER_TYPE _d3dDriverType    = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT     _backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    DXGI_MODE_DESC                               _mode;
    bool                                         _onResize = true;
    std::vector<std::vector<ID3D12CommandList*>> _commandLists;

    // TODO : temp 나중에 commandList manager생기고 삭제하기? 수정하기?
    ComPtr<ID3D12GraphicsCommandList> _commandList;
    ComPtr<ID3D12CommandAllocator>    _commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> _imguiCommandList;
    ComPtr<ID3D12CommandAllocator>    _imguiCommandAllocator;
    ComPtr<ID3D12GraphicsCommandList> _postProcessCommandList;
    ComPtr<ID3D12CommandAllocator>    _postProcessCommandAllocator;

    // compute 관련 command 객체들
    ComPtr<ID3D12CommandQueue>        _computeCommandQueue;
    ComPtr<ID3D12GraphicsCommandList> _computeCommandList;
    ComPtr<ID3D12CommandAllocator>    _computeCommandAllocator;

    // UploadBuffer 생명주기를 관리 할 UploadBuffer container
    std::vector<ComPtr<ID3D12Resource>> _uploadResources;


/// DXR
private:
    void CheckDXRSupport();
};