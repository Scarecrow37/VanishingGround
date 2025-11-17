#pragma once

class Device
{
public:
    Device()  = default;
    ~Device() = default;

public:
    ID3D12Device*                      GetDevice() const { return _device.Get(); }
    ComPtr<ID3D12Device5>              GetDevice5();
    ID3D12GraphicsCommandList*         GetCommandList() const { return _commandList.Get(); }
    ID3D12GraphicsCommandList*         GetComputeCommandList() const { return _computeCommandList.Get(); }
    ComPtr<ID3D12GraphicsCommandList4> GetCommandList4();
    const DXGI_MODE_DESC&              GetMode() const { return _mode; }
    UINT                               GetRTVDescriptorSize() { return _rtvDescriptorSize; }
    UINT                               GetCBVSRVUAVDescriptorSize() { return _cbvSrvUavDescriptorSize; }
    UINT                               GetDSVDescriptorSize() { return _dsvDescriptorSize; }
    DXGI_FORMAT                        GetBackBufferFormat() { return _backBufferFormat; }
    bool                               GetMSAAState() { return _4xMSAAState; }
    UINT                               GetMSAAQuality() { return _4xMSAAQuality; }
    UINT                               GetCurrentBackBufferIndex() { return _renderTargetIndex; }
    const D3D12_CPU_DESCRIPTOR_HANDLE& GetBackBufferHandle() const { return _renderTargetHandles[_renderTargetIndex]; }
    const D3D12_VIEWPORT&              GetMainViewport() const { return _mainViewport; }
    const D3D12_RECT&                  GetMainScissorRect() const { return _mainScissorRect; }
    const SIZE&                        GetResolution() const { return _resolution; }

public:
    void SetUpDevice(HWND hwnd, UINT width, UINT height, FeatureLevel feature);
    void Initialize();
    void Finalize();

public:
    void OnResize(UINT width, UINT height);
    void GPUSync();
    void FullGPUSync();

    void UploadResource(ComPtr<ID3D12Resource> uploadResource);
    void ResolveBackBuffer(ComPtr<ID3D12Resource> source);

    void ResetCommands();
    void ResetGraphicsCommnad();
    void ResetComputeCommands();

    void Execute();

public:
    void UpdateBuffer(ComPtr<ID3D12Resource>& buffer, const void* data, UINT64 size);
    void ClearBackBuffer(UINT flag, XMVECTOR color, float depth = 1.0f, UINT stencil = 0);
    void Flip();
    void CreateVertexBuffer(void* data, UINT size, UINT stride, ComPtr<ID3D12Resource>& buffer, D3D12_VERTEX_BUFFER_VIEW& view);
    void CreateVertexBuffer(ID3D12GraphicsCommandList* commandList, void* data, UINT size, UINT stride, ComPtr<ID3D12Resource>& buffer, D3D12_VERTEX_BUFFER_VIEW& view);
    void CreateIndexBuffer(void* data, UINT size, DXGI_FORMAT format, ComPtr<ID3D12Resource>& buffer, D3D12_INDEX_BUFFER_VIEW& view);
    void CreateIndexBuffer(ID3D12GraphicsCommandList* commandList, void* data, UINT size, DXGI_FORMAT format, ComPtr<ID3D12Resource>& buffer, D3D12_INDEX_BUFFER_VIEW& view);
    void CreateConstantBuffer(void* data, UINT size, ComPtr<ID3D12Resource>& buffer);
    void CreateDefaultBuffer(UINT size, ComPtr<ID3D12Resource>& buffer);
    void CreateCommandList(ComPtr<ID3D12CommandAllocator>& allocator, ComPtr<ID3D12GraphicsCommandList>& commandList, CommandType type);
    void CreateDefaultBuffer(UINT size, const D3D12_RESOURCE_FLAGS flags, const D3D12_RESOURCE_STATES initState, ComPtr<ID3D12Resource>& buffer);
    void CreateUploadBuffer(UINT size, const D3D12_RESOURCE_FLAGS flags, const D3D12_RESOURCE_STATES initState, ComPtr<ID3D12Resource>& buffer);
    ComPtr<ID3D12RootSignature> CreateRootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc);

private:
    void ResizeSwapChain();
    void CreateDeviceAndSwapChain(HWND hwnd, D3D_FEATURE_LEVEL feature);
    void CreateBackBuffer();
    void CreateBuffer(UINT size, ComPtr<ID3D12Resource>& buffer);

public:
    std::unique_ptr<GraphicsMemory> _graphicsMemory;
    ComPtr<ID3D12Device>            _device;
    ComPtr<IDXGIFactory4>           _dxgiFactory;
    ComPtr<IDXGISwapChain4>         _swapChain;

    ComPtr<ID3D12Resource>                   _swapChainBuffer[SWAPCHAIN_BUFFER_COUNT];
    UINT                                     _renderTargetIndex;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> _renderTargetHandles;

    D3D12_VIEWPORT _mainViewport;
    D3D12_RECT     _mainScissorRect;
    SIZE           _resolution;

    bool _4xMSAAState = false; // 4X MSAA enabled

    UINT _4xMSAAQuality           = 0; // quality level of 4X MSAA
    UINT _rtvDescriptorSize       = 0;
    UINT _dsvDescriptorSize       = 0;
    UINT _cbvSrvUavDescriptorSize = 0;

    D3D_DRIVER_TYPE _d3dDriverType    = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT     _backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    DXGI_MODE_DESC _mode;
    DXGI_MODE_DESC _newMode;

    bool           _onResize = true;

    ComPtr<ID3D12CommandAllocator>    _commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> _commandList;
    ComPtr<ID3D12CommandAllocator>    _computeCommandAllocator;
    ComPtr<ID3D12GraphicsCommandList> _computeCommandList;

    // UploadBuffer 생명주기를 관리 할 UploadBuffer container
    std::mutex                          _uploadBufferMutex;
    std::vector<ComPtr<ID3D12Resource>> _uploadBuffers;

    /// DXR
private:
    void CheckDXRSupport();
};