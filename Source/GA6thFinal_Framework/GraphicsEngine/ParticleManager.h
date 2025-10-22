#pragma once
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
    ComPtr<ID3D12PipelineState>       _computeRibbonInterpolatePSO;


    // -------------------------------------
    // [ Scene & Resource Management ]
    // -------------------------------------
    std::unordered_map<std::string, ParticleSceneResource>                         _sceneResources;
    std::unordered_map<std::string, UINT64>                                        _computeFences;
    std::unordered_map<EffectID, std::unordered_map<std::string, class ParticleEffect*>> _effectIDTable;

    // -------------------------------------
    // [ Configuration & State ]
    // -------------------------------------
    UINT _maxParticles = MAX_PARTICLE;
    UINT _maxEmitters = 100;
    int  _namingIndex = 0;
    UMPARTICLE_PROPERTY(float, _deltaScale, DeltaScale, 1.f);

    // -------------------------------------
    // [ Editor-related State ]
    // -------------------------------------
    class ParticleEffect* _editorCurrentEffect = nullptr;
    bool                  _editorRefreshFlag   = false;

public:
    // =================================================================================================================
    // [ 1. Constructor / Destructor & Initialize ]
    // =================================================================================================================
    ParticleManager();
    virtual ~ParticleManager();
    void Initialize(UINT maxParticles);

    // =================================================================================================================
    // [ 2. Scene Management ]
    // =================================================================================================================
    void AddSceneResource(std::string_view sceneName);

    // =================================================================================================================
    // [ 3. Effect Lifecycle Management ]
    // =================================================================================================================
    class ParticleEffect* RegisterEffect(EffectID id, const std::string& keyString, std::string_view sceneName);
    void                  DeleteEffect(EffectID id, const std::string& keyString, const std::string& sceneName);
    void                  PlayEffect(EffectID id, const std::string& keyString);
    void                  StopEffect(EffectID id, const std::string& keyString);
    class ParticleEffect* FindEffect(EffectID id, const std::string& keyString);
    // =================================================================================================================
    // [ 4. Emitter Management ]
    // =================================================================================================================
    class ParticleEmitter* RegisterEmitter(class ParticleEffect* effect, SIZE_T maxParticles = 10000,
                                           float             emissionRate    = 1000.f,
                                           float             emitterLifetime = 150.f,
                                           LocationShape     locatorShape    = LocationShape::SPHERE,
                                           Vector3           locationFactor  = Vector3(1, 1, 1),
                                           ParticleType      particleType    = ParticleType::SPRITE,
                                           const std::wstring& meshspritePath = L"");

    // =================================================================================================================
    // [ 5. Main Update Loop ]
    // =================================================================================================================
    void Update(const float deltaTime);

    // =================================================================================================================
    // [ 6. Getters ]
    // =================================================================================================================
    UINT                                         GetMaxCount();
    UINT                                         GetTotalCount(std::string_view sceneName);
    const std::vector<UINT>&                     GetActiveAlbedos(std::string_view sceneName);
    ID3D12Resource*                              GetComputeOutputResource(std::string_view sceneName);
    UINT                                         GetRibbonCount(std::string_view sceneName);
    const std::vector<std::vector<RibbonIndex>>& GetRibbonEmitterIndices(std::string_view sceneName);
    const std::vector<UINT>&                     GetActiveRibbonAlbedos(std::string_view sceneName);
    ID3D12Resource*                              GetRibbonOutputResource(std::string_view sceneName);
    UINT64                                       GetComputeFenceValue(std::string_view sceneName);

    // =================================================================================================================
    // [ 7. Setters ]
    // =================================================================================================================
    void SetActiveFlag(EffectID id, const std::string& keyString, bool flag);
    void SetRemoveFlag(EffectID id, const std::string& keyString, bool flag);
    void SetFollowBoneFlag(EffectID id, const std::string& keyString, bool* flag);
    void SetBoneMatrix(EffectID id, const std::string& keyString, const Matrix* boneMatrix);
    void SetCamera(std::string_view viewName);
    void SetCamera(std::shared_ptr<Camera> camera);

    // =================================================================================================================
    // [ 8. Editor-specific ]
    // =================================================================================================================
    class ParticleEffect* RegisterEffectOnEditor();
    void                  SetCurrentEditorEffect(class ParticleEffect* newEffect);
    class ParticleEffect* GetCurrentEditorEffect() { return _editorCurrentEffect; }
    void                  RefreshEditor();
    void                  UpdateEditorLifeCycle();
    UMPARTICLE_PROPERTY(bool, _isAutoRefresh, AutoRefresh, false);

    // =================================================================================================================
    // [ 9. Other Public Methods ]
    // =================================================================================================================
    void ChangeTexture();

private:
    // =================================================================================================================
    // [ 10. Internal Processing & DX12 Resource Management ]
    // =================================================================================================================
    void InitializeComputeCommandObject();
    void InitializeComputeCommandObject(ParticleSceneResource& scene);
    void InitializeParticleComputePSO();
    void CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, ComPtr<ID3D12Resource>& uploadResource,
                                UINT bufferSize, UINT stride);
    void CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride);
    void CreateUAVBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride);
    void CreateConstantBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize);
    void AwakeParticles(float deltaTime, const std::shared_ptr<ParticleUpdateResource>& scene);
    void UpdateAndCopyParticleResource(float deltaTime, const std::shared_ptr<ParticleUpdateResource>& scene);
    void DispatchSprite(float deltaTime, std::string sceneName);
    void DispatchRibbon(float deltaTime, std::string sceneName);
    void UpdateMvpConstant(float deltaTime, ParticleRenderResource* scene);
    void UpdateLifeCycle(float deltaTime);
    void RefreshCurrentEditorEffect();

    // =================================================================================================================
    // [ 11. Ribbon Tessellation ]
    // =================================================================================================================
    void CreateRibbonTessResources(struct ParticleSceneResource& scene, UINT tessFactorFixed) noexcept;
    void PrepareRibbonTessInputsCPU(struct ParticleSceneResource& scene, UINT tessFactorFixed, UINT& outTotalSegments);
    void DispatchRibbonInterpolateCS(struct ParticleSceneResource& scene, UINT tessFactorFixed, UINT totalSegments);
    void UploadStructuredBuffer(ID3D12GraphicsCommandList* cl, ComPtr<ID3D12Resource>& defaultRes,
                                ComPtr<ID3D12Resource>& uploadRes, const void* src, UINT byteSize);

};