#pragma once
class ParticleManager
{
public:
    ParticleManager(ID3D12Device* device, UINT maxParticles);

    void Initialize();
    void RegisterEffect();
    void DeleteEffect();
    void Update(const float deltaTime);
    
    //임시 render
    void Render();

private:
    void InitializeComputeCommandObject();
    void InitializeRenderCommandList();
    void InitializeComputeSyncObject();
    
    void InitializeParticleComputeShader();
    void InitializeParticleComputeRootSignature();
    void InitializeParticleComputePSO();

    void InitializeSortingComputeShader();
    void InitializeSortingComputeRootSignature();
    void InitializeSortingComputePSO();




    ComPtr<ID3D12Device>              _device;
    ComPtr<ID3D12CommandQueue>        _computeQueue;
    ComPtr<ID3D12CommandAllocator>    _computeAllocator;
    ComPtr<ID3D12GraphicsCommandList> _computeCommandList;

    ComPtr<ID3D12Fence> _computeFence;
    UINT64              _currentFenceValue = 0;
    HANDLE              _fenceEvent;

    ComPtr<ID3D12RootSignature> _computeSpriteRootSignature;
    ComPtr<ID3D12PipelineState> _computeAxialSpritePSO;
    ComPtr<ID3D12PipelineState> _computeSpritePSO;
    ComPtr<ID3DBlob>            _computeAxialSpriteShaderBlob;
    ComPtr<ID3DBlob>            _computeSpriteShaderBlob;

    ComPtr<ID3D12RootSignature> _computeMeshRootSignature;
    ComPtr<ID3D12PipelineState> _computeMeshPSO;
    ComPtr<ID3DBlob>            _computeMeshShaderBlob;



    ComPtr<ID3D12RootSignature> _sortingRootSignature;
    ComPtr<ID3D12PipelineState> _sortingPSO;
    ComPtr<ID3DBlob>            _sortingShaderBlob;



    ComPtr<ID3D12Resource>       _particleBuffers[2];
    ComPtr<ID3D12DescriptorHeap> _uavHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE  _uavHandles[2];

    ComPtr<ID3D12CommandAllocator>    _particleCommandAllocator;
    ComPtr<ID3D12GraphicsCommandList> _particleCommandList;


    UINT _currentBufferIndex;
    UINT _particleStride;
    UINT _maxParticles;

    std::vector<Matrix> _spriteBillboardMatrix;
    std::vector<Matrix> _spriteAxialBillboardMatrix;
    



};
