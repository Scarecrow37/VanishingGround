#pragma once
#include "Particle.h"
#include "ParticleHelper.h"
#include "ParticleEffectSerializer.h"
class ParticleManager
{
public:
    ParticleManager();
    virtual ~ParticleManager();


    void                   Initialize(UINT maxParticles);
    class ParticleEffect*  RegisterEffect();
    class ParticleEffect*  RegisterEffectOnEditor();


    /// <summary>
    /// Create new ParticleEffect.
    /// </summary>
    /// <param name="effect"></param>
    /// <param name="maxParticles"></param>
    /// <param name="emissionRate"></param>
    /// <param name="emitterLifetime"></param>
    /// <param name="locatorShape"></param>
    /// <param name="locationFactor"></param>
    /// <param name="particleType"></param>
    /// <returns></returns>
    class ParticleEmitter* RegisterEmitter(class ParticleEffect* effect, SIZE_T maxParticles = 10000,
                                           float emissionRate = 1000.f, float emitterLifetime = 150.f,
                                           LocationShape locatorShape   = LocationShape::SPHERE,
                                           Vector3       locationFactor = Vector3(1, 1, 1),
                                           ParticleType  particleType   = ParticleType::SPRITE,
                                          std::wstring  meshspritePath = L"../../../Resource/Assets/ParticleTexture/defaultSmoke.jpg");
    void                   DeleteEffect(class ParticleEffect* effect);
    void                   Update(const float deltaTime);
    void                   UpdateEffectLifeCycle();
    void                   UpdateEditorLifeCycle();
    void                   RefreshEditor();
    class ParticleEffect*  GetCurrentEditorEffect() { return _editorCurrentEffect; }
    void                   TurnOffEditorMode();
    void                   SetCurrentRenderScene(class RenderScene* renderScene);


    UMPARTICLE_PROPERTY(bool, _isAutoRefresh, AutoRefresh, false);


    UINT                                  GetTotalCount() const 
    { 
        if ("Game" == _currentRenderscene->_name || "Editor" == _currentRenderscene->_name)
            return _totalCount;
        else if ("ParticleEditor" == _currentRenderscene->_name)
            return _editorCount;
        else
            return 0;
    }

    UINT                                  GetMaxCount() const { return _maxParticles; }
    std::vector<Texture*> GetActiveAlbedos() const
    {
        if ("Game" == _currentRenderscene->_name || "Editor" == _currentRenderscene->_name)
            return _activeEmitterAlbedos;
        else // ("ParticleEditor" == _currentRenderscene->_name)
            return _activeEditorAlbedos;
    }

    ID3D12Resource*                       GetComputeOutputResource() 
    {
        if ("Game" == _currentRenderscene->_name || "Editor" == _currentRenderscene->_name)
            return _particleOutputBuffer.Get();
        else if ("ParticleEditor" == _currentRenderscene->_name)
            return _editorOutputBuffer.Get();
        else
            return nullptr;
    }
    ID3D12GraphicsCommandList*            GetRenderCommandList() { return _renderCommandList.Get(); }
    std::vector<class ParticleEffect*>&   GetEffectList() { return _particleEffects; }

public:
    ParticleEffectSerializer ParticleSerializer;



public:
    void SetCamera(std::string_view viewName);
    void SetCamera(std::shared_ptr<Camera> camera);
    void SetCurrentEditorEffect(class ParticleEffect* newEffect);

private:
    void InitializeComputeCommandObject();
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
    void CopyActiveParticlesEditorMode();

    void DispatchParticleCompute(float deltaTime);
    void UpdateParticleResources(float deltaTime);
    void CopyFromUploadBuffer();

    void DispatchParticleComputeEditorMode(float deltaTime);
    void UpdateParticleResourcesEditorMode(float deltaTime);
    void CopyFromUploadBufferEditorMode();


    class RenderScene*      _currentRenderscene = nullptr;
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
    ComPtr<ID3D12Resource> _mvpConstantBuffer;    // MVP 상수 버퍼 (CBV - b0)
    ComPtr<ID3D12Resource> _particleInputUploadBuffer;
    ComPtr<ID3D12Resource> _emitterInfoUploadBuffer;

    ComPtr<ID3D12Resource> _editorParticleInputBuffer; // 입력 파티클 데이터 (SRV - t0)
    ComPtr<ID3D12Resource> _editorEmitterInfoBuffer;   // 에미터 정보 (SRV - t1)
    ComPtr<ID3D12Resource> _editorOutputBuffer;        // 출력 파티클 데이터 (UAV - u0)
    ComPtr<ID3D12Resource> _editorMvpConstantBuffer;   // MVP 상수 버퍼 (CBV - b0)
    ComPtr<ID3D12Resource> _editorParticleInputUploadBuffer;
    ComPtr<ID3D12Resource> _editorEmitterInfoUploadBuffer;



    void RefreshCurrentEditorEffect();
    bool _editorRefreshFlag = false;




private:
    UINT _currentBufferIndex;
    UINT _particleStride;
    UINT _maxParticles;
    UINT _maxEmitters = 100;

    bool pauseFlag = false;

    UINT _particleEmitterCount = 0;

    class ParticleEffect* _editorCurrentEffect = nullptr;
    class ParticleEffect* _editorCurrentEffectInstance = nullptr;

    std::vector<class ParticleEffect*>    _activePariticleEffects;
    std::vector<class ParticleEffect*>    _particleEffects;




    std::vector<class Particle>           _totalParticles;
    std::vector<class Particle>           _editorTotalParticles;
    
    std::vector<EmitterInfo>              _emitterMatrix;
    std::vector<EmitterInfo>              _editorEmitterMatrix;

    std::vector<Texture*> _activeEmitterAlbedos;
    std::vector<Texture*> _activeEditorAlbedos;


    std::vector<Texture*> _activeEmitterNormals;

    UINT _totalCount = 0;
    UINT _editorCount = 0;


    float _elapsedTimer = 0.f;
    int   nameingIndex  = 0;


   UMPARTICLE_PROPERTY(float, _deltaScale, DeltaScale, 1.f);


};
