#pragma once
#include "Particle.h"
#include "ParticleHelper.h"

class ParticleManager
{
    using EffectID = void*;
private:
    // -------------------------------------
    // [ Core DX12 Objects ]
    // -------------------------------------
    ComPtr<ID3D12CommandAllocator>    _computeAllocator;
    ComPtr<ID3D12GraphicsCommandList> _computeCommandList;
    ComPtr<ID3D12PipelineState>       _computeSpritePSO;
    ComPtr<ID3D12PipelineState>       _computeRibbonPSO;
    ComPtr<ID3D12DescriptorHeap> _cbvSrvUavHeap;


    // -------------------------------------
    // [ Scene & Resource Management ]
    // -------------------------------------
    std::unordered_map<std::string, ParticleSceneResource> _sceneResources;
    std::unordered_set<ParticleUpdateResource*>            _particleUpdateResources;
    std::unordered_map<std::string, UINT64> _computeFences;
    std::unordered_map<EffectID, std::unordered_map<std::string, ParticleEffect*>> _effectIDTable;

    // -------------------------------------
    // [ Configuration & State ]
    // -------------------------------------
    UINT _maxParticles;
    UINT _maxEmitters = 100;
    int   nameingIndex  = 0;
    UMPARTICLE_PROPERTY(float, _deltaScale, DeltaScale, 1.f);
    UINT _particleStride;
    UINT _currentBufferIndex;
    bool pauseFlag = false;
    float                   lastFrameTime = 0;

    // -------------------------------------
    // [ Editor-related State ]
    // -------------------------------------
    class ParticleEffect* _editorCurrentEffect = nullptr;
    bool _editorRefreshFlag = false;
    
public:
    // =================================================================================================================
    // [ 1. Constructor / Destructor & Initialize ]
    // =================================================================================================================
    ParticleManager();
    virtual ~ParticleManager();
    void                   Initialize(UINT maxParticles);

    // =================================================================================================================
    // [ 2. Scene Management ]
    // =================================================================================================================
    void AddSceneResource(std::string_view sceneName);
    void AddSceneResource(std::string_view sceneName, std::string_view sharedFrom);

    // =================================================================================================================
    // [ 3. Effect Lifecycle Management ]
    // =================================================================================================================
    class ParticleEffect*  RegisterEffect(EffectID id, const std::string& keyString, std::string_view sceneName);
    void                   DeleteEffect(EffectID id, const std::string& keyString, const std::string& sceneName);
    void                   PlayEffect(EffectID id, const std::string& keyString);
    void                   StopEffect(EffectID id, const std::string& keyString);

    // =================================================================================================================
    // [ 4. Emitter Management ]
    // =================================================================================================================
    class ParticleEmitter* RegisterEmitter(class ParticleEffect* effect, SIZE_T maxParticles = 10000, float emissionRate = 1000.f,
                                           float             emitterLifetime = 150.f,
                                           LocationShape     locatorShape    = LocationShape::SPHERE,
                                           Vector3 locationFactor = Vector3(1, 1, 1), ParticleType particleType = ParticleType::SPRITE,
                                           std::wstring_view meshspritePath = L"");

    // =================================================================================================================
    // [ 5. Main Update Loop ]
    // =================================================================================================================
    void                   Update(const float deltaTime);

    // =================================================================================================================
    // [ 6. Getters ]
    // =================================================================================================================
    UINT                  GetMaxCount();
    UINT                  GetTotalCount(std::string_view sceneName);
    std::vector<Texture*> GetActiveAlbedos(std::string_view sceneName);
    ID3D12Resource*       GetComputeOutputResource(std::string_view sceneName);
    UINT                                  GetRibbonCount(std::string_view sceneName);
    std::vector<std::vector<RibbonIndex>> GetRibbonEmitterIndices(std::string_view sceneName);
    std::vector<Texture*>                 GetActiveRibbonAlbedos(std::string_view sceneName);
    ID3D12Resource*                       GetRibbonOutputResource(std::string_view sceneName);
    UINT64                                GetComputeFenceValue(std::string_view sceneName);

    // =================================================================================================================
    // [ 7. Setters ]
    // =================================================================================================================
    void                   SetActiveFlag(EffectID id, const std::string& keyString, bool flag);
    void                   SetRemoveFlag(EffectID id, const std::string& keyString, bool flag);
    void                   SetFollowBoneFlag(EffectID id, const std::string& keyString, bool* flag);
    void                   SetBoneMatrix(EffectID id, const std::string& keyString, const Matrix* boneMatrix);
    void SetCamera(std::string_view viewName);
    void SetCamera(std::shared_ptr<Camera> camera);

    // =================================================================================================================
    // [ 8. Editor-specific ]
    // =================================================================================================================
    class ParticleEffect*  RegisterEffectOnEditor();
    void SetCurrentEditorEffect(class ParticleEffect* newEffect);
    class ParticleEffect*  GetCurrentEditorEffect() { return _editorCurrentEffect; }
    void                   RefreshEditor();
    void                   UpdateEditorLifeCycle();
    UMPARTICLE_PROPERTY(bool, _isAutoRefresh, AutoRefresh, false);
    void                   TurnOffEditorMode();

    // =================================================================================================================
    // [ 9. Other Public Methods ]
    // =================================================================================================================
    void                   ChangeTexture();

private:
    // =================================================================================================================
    // [ 10. Internal Processing & DX12 Resource Management ]
    // =================================================================================================================
    void InitializeComputeCommandObject();
    void InitializeComputeCommandObject(std::string_view sceneName);
    void InitializeParticleComputePSO();
    void InitializeDescriptorHeap();
    void CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, ComPtr<ID3D12Resource>& uploadResource,
                                UINT bufferSize, UINT stride);
    void CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride);
    void CreateUAVBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride);
    void CreateConstantBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize);
    void AwakeParticles(float deltaTime, ParticleUpdateResource* targetScene);
    void UpdateAndCopyParticleResource(float deltaTime, ParticleUpdateResource* scene);
    void DispatchSprite(float deltaTime, std::string sceneName);
    void DispatchRibbon(float deltaTime, std::string sceneName);
    void UpdateMvpConstant(float deltaTime, ParticleRenderResource* scene);
    void UpdateLifeCycle(float deltaTime);
    void RefreshCurrentEditorEffect();


private:
    // Legacy members from ParticleManager_1, kept for compatibility
    UINT _particleEmitterCount = 0;
    std::vector<class ParticleEffect*>    _particleEffects;
    std::vector<class Particle> _totalParticles;
    std::vector<class Particle> _editorTotalParticles;
    std::vector<EmitterInfo>    _emitterMatrix;
    std::vector<EmitterInfo>    _editorEmitterMatrix;
    std::vector<Texture*>       _activeEmitterAlbedos;
    std::vector<Texture*>       _activeEditorAlbedos;
    std::vector<class Particle> _ribbonTotalParticles;
    std::vector<class Particle> _ribbonEditorTotalParticles;
    std::vector<EmitterInfo>    _ribbonEmitterMatrix;
    std::vector<EmitterInfo>    _ribbonEditorEmitterMatrix;
    std::vector<Texture*>       _ribbonActiveEmitterAlbedos;
    std::vector<Texture*>       _ribbonActiveEditorAlbedos;
    std::vector<std::vector<RibbonIndex>> _ribbonIndices;
    std::vector<std::vector<RibbonIndex>> _ribbonEditorIndices;
    UINT _totalCount = 0;
    UINT _editorCount = 0;
    UINT _ribbonTotalCount  = 0;
    UINT _ribbonEditorCount = 0;
    float _elapsedTimer = 0.f;
    std::shared_ptr<Camera> _camera;
    UINT _descriptorSize;
};