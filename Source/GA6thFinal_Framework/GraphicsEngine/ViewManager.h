#pragma once

class ViewManager
{
public:
    enum class Type
    {
        SHADER_RESOURCE,
        RENDER_TARGET,
        DEPTH_STENCIL,
        VERTEX_BUFFER_SHADER_RESOURCE,
        INDEX_BUFFER_SHADER_RESOURCE
    };

public:
    ViewManager()  = default;
    ~ViewManager() = default;

public:
    ID3D12DescriptorHeap* GetShaderResourceHeap() const { return _shaderResourceHeap.Get(); }
    ID3D12DescriptorHeap* GetRenderTargetHeap() const { return _renderTargetHeap.Get(); }
    ID3D12DescriptorHeap* GetDepthStencilHeap() const { return _depthStencilHeap.Get(); }
    const UINT64          GetVertexBufferSrvPtr();
    const UINT64          GetIndexBufferSrvPtr();
    UINT                  GetNumVertexBuffer() { return _numVertexSrv; }
    UINT                  GetNumIndexBuffer() { return _numIndexSrv; }

public:
    void Initialize();
    void AddDescriptorHeap(const ViewManager::Type type, D3D12_CPU_DESCRIPTOR_HANDLE& handle, UINT* ID = nullptr);
    void AddDescriptorHeap(const ViewManager::Type type, DescriptorHandles& handles, UINT* ID = nullptr);
    void ReturnShaderResourceDescriptorHeap(UINT ID);

private:
    ComPtr<ID3D12DescriptorHeap> _shaderResourceHeap;
    ComPtr<ID3D12DescriptorHeap> _renderTargetHeap;
    ComPtr<ID3D12DescriptorHeap> _depthStencilHeap;

    UINT _shaderResourceDescriptorSize{0};
    UINT _renderTargetDescriptorSize{0};
    UINT _depthStencilDescriptorSize{0};
    UINT _numShaderResource{1};
    UINT _numRenderTarget{0};
    UINT _numDepthStencil{0};

    UINT _vertexSrvStartIndex{1000};
    UINT _indexSrvStartIndex{3000};
    UINT _numVertexSrv{0};
    UINT _numIndexSrv{0};

    std::queue<UINT> _availableShaderResourceIDs;
};
