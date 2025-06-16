#pragma once
#include "Particle.h"
#include "ParticleHelper.h"

class ParticleManager
{
public:
    ParticleManager();

    void                   Initialize(UINT maxParticles);
    class ParticleEffect*  RegisterEffect();
    class ParticleEmitter* RegisterEmitter(class ParticleEffect* effect, SIZE_T maxParticles = 100000,
                                           float emissionRate = 1000.f, float emitterLifetime = 150.f,
                                           LocationShape locatorShape   = LocationShape::SPHERE,
                                           Vector3       locationFactor = Vector3(1, 1, 1));
    void                   DeleteEffect(UINT);
    void                   Update(const float deltaTime);

    // 임시 render
    void Render();

    UINT                                  GetTotalCount() const { return _totalCount; }
    UINT                                  GetMaxCount() const { return _maxParticles; }
    std::vector<std::shared_ptr<Texture>> GetActiveAlbedos() const { return _activeEmitterAlbedos; }
    //임시
    bool isSorted = true;
    //
    ID3D12Resource*                GetComputeOutputResource() 
    {
        if (isSorted)
            return _sortedOutputBuffer.Get();
        else
            return _particleOutputBuffer.Get(); 
    }
    ID3D12GraphicsCommandList*     GetRenderCommandList() { return _renderCommandList.Get(); }
    void                                  ResetRenderCommandObject();

public:
    void SetCamera(std::string_view viewName);
    void SetCamera(std::shared_ptr<Camera> camera);

private:
    void InitializeComputeCommandObject();
    void IntializeGraphicsCommandObject();

    void InitializeParticleComputeShader();
    void InitializeParticleComputeRootSignature();
    void InitializeParticleComputePSO();

    void InitializeDescriptorHeap();

    void CreateParticleResources();
    void CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, ComPtr<ID3D12Resource>& uploadResource,
                                UINT bufferSize, UINT stride);
    void CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride);
    void CreateUAVBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride);
    void CreateConstantBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize);

    void CreateDescriptors();

    void DispatchParticleCompute(float deltaTime);
    void UpdateParticleResources(float deltaTime);

    void CopyFromUploadBuffer();

    class RenderScene*      _currentRenderscene;
    std::shared_ptr<Camera> _camera;
    float                   lastFrameTime = 0;

    ComPtr<ID3D12CommandAllocator>    _renderAllocator;
    ComPtr<ID3D12GraphicsCommandList> _renderCommandList;

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
    ComPtr<ID3D12Resource> _sortedOutputBuffer; // 출력 파티클 데이터 (UAV - u0)
    ComPtr<ID3D12Resource> _mvpConstantBuffer;    // MVP 상수 버퍼 (CBV - b0)

    // 업로드 버퍼
    ComPtr<ID3D12Resource> _particleInputUploadBuffer;
    ComPtr<ID3D12Resource> _emitterInfoUploadBuffer;

private:
    ComPtr<ID3D12CommandAllocator>    _depthExtractAllocator;
    ComPtr<ID3D12GraphicsCommandList> _depthExtractCommandList;

    ComPtr<ID3D12CommandAllocator>    _radixSortAllocator;
    ComPtr<ID3D12GraphicsCommandList> _radixSortCommandList;

    ComPtr<ID3D12CommandAllocator>    _reorderAllocator;
    ComPtr<ID3D12GraphicsCommandList> _reorderCommandList;

    ComPtr<ID3D12CommandAllocator>    _validateAllocator;
    ComPtr<ID3D12GraphicsCommandList> _validateCommandList;





    // Radix Sort 관련 리소스
    ComPtr<ID3D12Resource> _sortKeysBuffer;      // 정렬 키 버퍼
    ComPtr<ID3D12Resource> _sortValuesBuffer;    // 정렬 값 버퍼
    ComPtr<ID3D12Resource> _sortKeysAltBuffer;   // 대체 키 버퍼 (ping-pong)
    ComPtr<ID3D12Resource> _sortValuesAltBuffer; // 대체 값 버퍼 (ping-pong)
    ComPtr<ID3D12Resource> _histogramBuffer;     // 히스토그램 버퍼
    ComPtr<ID3D12Resource> _prefixSumBuffer;     // 접두사 합 버퍼
    std::vector<ComPtr<ID3D12Resource>> _sortConstantBuffer;  // 정렬 상수 버퍼

    // Radix Sort 컴퓨트 셰이더
    ComPtr<ID3DBlob> _depthExtractShaderBlob;
    ComPtr<ID3DBlob> _histogramShaderBlob;
    ComPtr<ID3DBlob> _prefixSumShaderBlob;
    ComPtr<ID3DBlob> _scatterShaderBlob;
    ComPtr<ID3DBlob> _reorderShaderBlob;

    // 파이프라인 상태
    ComPtr<ID3D12PipelineState> _depthExtractPSO;
    ComPtr<ID3D12PipelineState> _histogramPSO;
    ComPtr<ID3D12PipelineState> _prefixSumPSO;
    ComPtr<ID3D12PipelineState> _scatterPSO;
    ComPtr<ID3D12PipelineState> _reorderPSO;

    // 루트 시그니처
    ComPtr<ID3D12RootSignature> _radixSortRootSignature;

    // 새로운 메서드 선언
    void InitializeRadixSortShaders();
    void InitializeRadixSortRootSignature();
    void InitializeRadixSortPSO();
    void CreateRadixSortResources();
    void PerformRadixSort();
    void ExtractDepthKeys();
    void ReorderParticleOutput();

private:
    UINT _currentBufferIndex;
    UINT _particleStride;
    UINT _maxParticles;
    UINT _maxEmitters = 100;

    bool pauseFlag = false;

    UINT _particleEmitterCount = 0;

    std::vector<class ParticleEffect*>    _pariticleEffects;
    std::vector<class Particle>           _totalParticles;
    std::vector<EmitterInfo>              _emitterMatrix;
    std::vector<std::shared_ptr<Texture>> _activeEmitterAlbedos;
    std::vector<std::shared_ptr<Texture>> _activeEmitterNormals;

    UINT _totalCount = 0;
};
