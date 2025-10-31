#pragma once
#include "ParticleHelper.h"

class ParticleManager
{
    using EffectID       = void*;
    using EffectCallback = std::function<void(void)>;

private:
    // -------------------------------------
    // [ Core DX12 Objects ]
    // -------------------------------------
    ComPtr<ID3D12CommandAllocator>    _computeAllocator;
    ComPtr<ID3D12GraphicsCommandList> _computeCommandList;

    ComPtr<ID3D12RootSignature> _computeSpriteRootSignature;
    ComPtr<ID3D12PipelineState> _computeSpritePSO;

    ComPtr<ID3D12RootSignature> _computeRibbonRootSignature;
    ComPtr<ID3D12PipelineState> _computeRibbonPSO;

    // -------------------------------------
    // [ Scene & Resource Management ]
    // -------------------------------------
    std::unordered_map<std::string, ParticleSceneResource>                                             _sceneResources;
    std::unordered_map<std::string, UINT64>                                                            _computeFences;
    std::unordered_map<EffectID, std::unordered_map<std::string, std::weak_ptr<class ParticleEffect>>> _effectIDTable;

    // -------------------------------------
    // [ Configuration & State ]
    // -------------------------------------
    UINT _maxParticles = MAX_PARTICLE;
    UINT _maxEmitters  = 1024;

    // Editor
    class ParticleEffect* _editorCurrentEffect = nullptr;
    bool                  _editorRefreshFlag   = false;

    // Naming
    size_t _namingIndex = 0;

public:
    ParticleManager();
    ~ParticleManager();

    void Initialize(UINT maxParticles = MAX_PARTICLE);

    // -------------------------------------
    // [ Scene Management ]
    // -------------------------------------
    void AddScene(const std::string& sceneName);
    void RemoveScene(const std::string& sceneName);

    // -------------------------------------
    // [ Effect Lifecycle ]
    // -------------------------------------
    class ParticleEffect* RegisterEffect(EffectID id, const std::string& keyString, std::string_view sceneName);
    void                  DeleteEffect(EffectID id, const std::string& keyString, const std::string& sceneName);
    class ParticleEffect* FindEffect(EffectID id, const std::string& keyString);

    // -------------------------------------
    // [ Update / Dispatch / Copy ]
    // -------------------------------------
    void Update(float deltaTime, const std::string& sceneName);
    void Dispatch(const std::string& sceneName);
    void CopyToGPU(const std::string& sceneName);

    // -------------------------------------
    // [ Controls ]
    // -------------------------------------
    void PlayEffect(EffectID id, const std::string& keyString);
    void PlayEffect(EffectID id, const std::string& keyString, EffectCallback callback);
    void StopEffect(EffectID id, const std::string& keyString);
    void StopEffectAndDisable(EffectID id, const std::string& keyString);
    void PauseEffect(EffectID id, const std::string& keyString);
    void ResumeEffect(EffectID id, const std::string& keyString);

    // Misc setters routed to the effect
    void SetActive(EffectID id, const std::string& keyString, bool v);
    void SetPosition(EffectID id, const std::string& keyString, const Vector3& p);
    void SetRotation(EffectID id, const std::string& keyString, const Quaternion& q);
    void SetScale(EffectID id, const std::string& keyString, const Vector3& s);
    void SetBoneFollowFlag(EffectID id, const std::string& keyString, bool* flag);
    void SetBoneMatrix(EffectID id, const std::string& keyString, const Matrix* boneMatrix);

    // -------------------------------------
    // [ Editor ]
    // -------------------------------------
    class ParticleEffect* RegisterEffectOnEditor();
    void                  SetCurrentEditorEffect(class ParticleEffect* newEffect);
    void                  RefreshEditor();
    void                  UpdateEditorLifeCycle();

private:
    // internals (생략된 DX12/버퍼 유틸 등은 기존 그대로)
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
};
