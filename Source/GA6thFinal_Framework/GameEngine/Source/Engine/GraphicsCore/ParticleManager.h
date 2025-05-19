#pragma once
#include "ParticleHelper.h"
#include "Particle.h"

class ParticleManager
{
public:
    ParticleManager( );

    void Initialize(UINT maxParticles);
    void RegisterEffect();
    void DeleteEffect(UINT);
    void Update(const float deltaTime);
    
    //임시 render
    void Render();


    UINT GetTotalCount() const { return _totalCount; }

public:
    void                SetCamera(std::string_view viewName);


private:
    void InitializeComputeCommandObject();
    void InitializeRenderCommandList();
    
    void InitializeParticleComputeShader();
    void InitializeParticleComputeRootSignature();
    void InitializeParticleComputePSO();

    void InitializeSortingComputeShader();
    void InitializeSortingComputeRootSignature();
    void InitializeSortingComputePSO();
    
    void InitializeDescriptorHeap();
    void SetSpriteComputeState();

    void CreateParticleResources();
    void CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, ComPtr<ID3D12Resource>& uploadResource,
                                UINT bufferSize, UINT stride);
    void CreateUAVBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride);
    void CreateConstantBuffer(ComPtr<ID3D12Resource>& resource, ComPtr<ID3D12Resource>& uploadResource,
                              UINT bufferSize);
    void CreateDescriptors();

    void DispatchParticleCompute(float deltaTime);
    void UpdateParticleResources(float deltaTime);

    
        std::shared_ptr<Camera> _camera;


    ComPtr<ID3D12CommandQueue>        _computeQueue;
    ComPtr<ID3D12CommandAllocator>    _computeAllocator;
    ComPtr<ID3D12GraphicsCommandList> _computeCommandList;

    ComPtr<ID3D12RootSignature> _computeSpriteRootSignature;
    ComPtr<ID3D12PipelineState> _computeAxialSpritePSO;
    ComPtr<ID3D12PipelineState> _computeSpritePSO;
    ComPtr<ID3DBlob>            _computeAxialSpriteShaderBlob;
    ComPtr<ID3DBlob>            _computeSpriteShaderBlob;

    ComPtr<ID3D12RootSignature> _computeMeshRootSignature;
    ComPtr<ID3D12PipelineState> _computeMeshPSO;
    ComPtr<ID3DBlob>            _computeMeshShaderBlob;

    ComPtr<ID3D12DescriptorHeap> _cbvSrvUavHeap;
    UINT                         _descriptorSize;

    // 리소스 버퍼
    ComPtr<ID3D12Resource> _particleInputBuffer;  // 입력 파티클 데이터 (SRV - t0)
    ComPtr<ID3D12Resource> _emitterInfoBuffer;    // 에미터 정보 (SRV - t1)
    ComPtr<ID3D12Resource> _particleOutputBuffer; // 출력 파티클 데이터 (UAV - u0)
    ComPtr<ID3D12Resource> _mvpConstantBuffer;    // MVP 상수 버퍼 (CBV - b0)
    
    // 업로드 버퍼
    ComPtr<ID3D12Resource> _particleInputUploadBuffer;
    ComPtr<ID3D12Resource> _emitterInfoUploadBuffer;
    ComPtr<ID3D12Resource> _mvpUploadBuffer;


    ComPtr<ID3D12RootSignature> _sortingRootSignature;
    ComPtr<ID3D12PipelineState> _sortingPSO;
    ComPtr<ID3DBlob>            _sortingShaderBlob;


    ComPtr<ID3D12CommandAllocator>    _particleCommandAllocator;
    ComPtr<ID3D12GraphicsCommandList> _particleCommandList;


    UINT _currentBufferIndex;
    UINT _particleStride;
    UINT _maxParticles;
    UINT _maxEmitters = 100;

    //std::vector<Matrix> _spriteBillboardMatrix;
    //std::vector<Matrix> _spriteAxialBillboardMatrix;
    //
    UINT _particleEmitterCount = 0;
    
    std::vector<class ParticleEffect*> _pariticleEffects;
    std::vector<class Particle>       _totalParticles;
    std::vector<EmitterInfo>    _emitterMatrix;
    UINT                               _totalCount = 0;

};
