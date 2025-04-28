#pragma once

class Device
{
public:
    Device()  = default;
    ~Device() = default;

public:
    ComPtr<ID3D12Device>              GetDevice() const { return _device; }
    ComPtr<ID3D12CommandQueue>        GetCommandQueue() const { return _commandQueue; }
    ComPtr<ID3D12GraphicsCommandList> GetCommandList() const { return _commandList; }
    const DXGI_MODE_DESC&             GetMode() const { return _mode; }
    float                             GetEngineTime();

public:
    void SetUpDevice(HWND hwnd, UINT width, UINT height, FEATURE_LEVEL feature);
    void Initialize();
    void Finalize();

public:
    void OnResize(UINT width, UINT height);
    void GPUSync();
    void SetCurrentPipelineState(ComPtr<ID3D12PipelineState> pipelineState);
    void SetBackBuffer();
    void RegisterCommand(ID3D12GraphicsCommandList* commandList);
    void ExecuteCommand();
    void ResetCommands();
    void ResolveBackBuffer(ComPtr<ID3D12Resource> source);

    UINT GetRTVDescriptorSize() { return _rtvDescriptorSize; }
    UINT GetCBVSRVUAVDescriptorSize() { return _cbvSrvUavDescriptorSize; }
    UINT GetDSVDescriptorSize() { return _dsvDescriptorSize; }

    DXGI_FORMAT GetBackBufferFormat() { return _backBufferFormat; }
    DXGI_FORMAT GetDepthStencilFormat() { return _depthStencilFormat; }

    bool GetMSAAState() { return _4xMSAAState; }
    UINT GetMSAAQuality() { return _4xMSAAQuality; }
    UINT GetCurrentBackBufferIndex() { return _renderTargetIndex; }
    D3D12_VIEWPORT GetMainViewport() { return _mainViewport; }
    HRESULT UpdateBuffer(ComPtr<ID3D12Resource>& buffer, void* data, UINT size);

    HRESULT ClearBackBuffer(XMVECTOR color);
    HRESULT ClearBackBuffer(UINT flag, XMVECTOR color, float depth = 1.0f, UINT stencil = 0);
    HRESULT Flip();

    HRESULT CreateVertexBuffer(void* data, UINT size, UINT stride,  ComPtr<ID3D12Resource>& buffer,
                                D3D12_VERTEX_BUFFER_VIEW& view);
    HRESULT CreateIndexBuffer(void* data, UINT size, DXGI_FORMAT format,  ComPtr<ID3D12Resource>& buffer,
                               D3D12_INDEX_BUFFER_VIEW& view);
    HRESULT CreateConstantBuffer(void* data, UINT size,  ComPtr<ID3D12Resource>& buffer);
    HRESULT CreateDefaultBuffer(UINT size,  ComPtr<ID3D12Resource>& buffer);

    HRESULT CreateCommandList(ComPtr<ID3D12CommandAllocator>& allocator, 
                              ComPtr<ID3D12GraphicsCommandList>& commandList,
                              COMMAND_TYPE type);

private:
    void    SetViewPort();
    void    CreateDeviceAndSwapChain(HWND hwnd, D3D_FEATURE_LEVEL feature);
    void    CreateCommandQueue();
    void    CreateSyncObject();
    void    CreateRenderTarget();
    void    CreateDepthStencil();
    HRESULT CreateBuffer(UINT size, ComPtr<ID3D12Resource>& buffer);

private:
    ComPtr<ID3D12Device>       _device;
    ComPtr<IDXGIFactory4>      _dxgiFactory;
    ComPtr<IDXGISwapChain4>    _swapChain;
    ComPtr<ID3D12CommandQueue> _commandQueue;
    ComPtr<ID3D12Fence>        _fence;
    UINT64                     _fenceValue = 0;
    HANDLE                     _fenceEvent;

    ComPtr<ID3D12Resource>                   _swapChainBuffer[SWAPCHAIN_BUFFER_COUNT];
    ComPtr<ID3D12Resource>                   _depthStencilBuffer;
    UINT                                     _renderTargetIndex;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> _renderTargetHandles;
    D3D12_CPU_DESCRIPTOR_HANDLE              _depthStencilHandle;
    ComPtr<ID3D12PipelineState>              _currentPipelineState;

    D3D12_VIEWPORT _mainViewport;
    D3D12_RECT     _mainrRect;

    bool _4xMSAAState = false; // 4X MSAA enabled

    UINT _4xMSAAQuality           = 0; // quality level of 4X MSAA
    UINT _rtvDescriptorSize       = 0;
    UINT _dsvDescriptorSize       = 0;
    UINT _cbvSrvUavDescriptorSize = 0;

    D3D_DRIVER_TYPE _d3dDriverType      = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT     _backBufferFormat   = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT     _depthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    DXGI_MODE_DESC                  _mode;
    std::vector<ID3D12CommandList*> _commandLists;

    // TODO : temp 나중에 commandList manager생기고 삭제하기? 수정하기?
    ComPtr<ID3D12GraphicsCommandList> _commandList;
    ComPtr<ID3D12CommandAllocator>    _commandAllocator;
};