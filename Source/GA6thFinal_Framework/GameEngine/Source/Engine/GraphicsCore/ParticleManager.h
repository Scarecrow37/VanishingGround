#pragma once
#include "Particle.h"
#include "ParticleHelper.h"

class ParticleManager
{
public:
    ParticleManager();
    float elapsedtimer = 0.f;


    void                   Initialize(UINT maxParticles);
    class ParticleEffect*  RegisterEffect();
    class ParticleEmitter* RegisterEmitter(class ParticleEffect* effect, SIZE_T maxParticles = 100000,
                                           float emissionRate = 1000.f, float emitterLifetime = 150.f,
                                           LocationShape locatorShape   = LocationShape::SPHERE,
                                           Vector3       locationFactor = Vector3(1, 1, 1));
    void                   DeleteEffect(UINT);
    void                   Update(const float deltaTime);


    UINT                                  GetTotalCount() const { return _totalCount; }
    UINT                                  GetMaxCount() const { return _maxParticles; }
    std::vector<std::shared_ptr<Texture>> GetActiveAlbedos() const { return _activeEmitterAlbedos; }
    //임시
    bool isSorted = true;
    //
    ID3D12Resource*                GetComputeOutputResource() 
    {
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

    void CopyActiveParticles();

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


    float _elapsedTimer = 0.f;



};
