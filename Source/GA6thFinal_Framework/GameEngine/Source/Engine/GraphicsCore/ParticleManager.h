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
    ComPtr<ID3D12Device>              _device;
    ComPtr<ID3D12CommandQueue>        _computeQueue;
    ComPtr<ID3D12CommandAllocator>    _computeAllocator;
    ComPtr<ID3D12GraphicsCommandList> _computeCmdList;

    ComPtr<ID3D12Fence> _computeFence;
    UINT64              _currentFenceValue = 0;
    HANDLE              _fenceEvent;

    ComPtr<ID3D12RootSignature> _computeRootSignature;
    ComPtr<ID3D12PipelineState> _computePSO;
    
    ComPtr<ID3D12Resource>       _particleBuffers[2];
    ComPtr<ID3D12DescriptorHeap> _uavHeap;
    D3D12_CPU_DESCRIPTOR_HANDLE  _uavHandles[2];

        UINT _currentBufferIndex;
    UINT _particleStride;
    UINT _maxParticles;

    std::vector<Matrix> _spriteBillboardMatrix;
    std::vector<Matrix> _spriteAxialBillboardMatrix;
    



};
