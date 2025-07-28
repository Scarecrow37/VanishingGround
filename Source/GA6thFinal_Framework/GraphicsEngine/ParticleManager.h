#pragma once
#include "Particle.h"
#include "ParticleHelper.h"

class ParticleManager
{

public:
    ParticleManager();
    virtual ~ParticleManager();


    void                   Initialize(UINT maxParticles);
    class ParticleEffect* RegisterEffect(std::string_view sceneName);
    class ParticleEffect*  RegisterEffectOnEditor();
    void                   ChangeTexture();

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
    class ParticleEmitter* RegisterEmitter(
        class ParticleEffect* effect, SIZE_T maxParticles = 10000, float emissionRate = 1000.f,
        float emitterLifetime = 150.f, LocationShape locatorShape = LocationShape::SPHERE,
        Vector3 locationFactor = Vector3(1, 1, 1), ParticleType particleType = ParticleType::SPRITE,
        std::wstring_view meshspritePath = L"../../../Resource/Assets/VFX/ParticleTexture/defaultSmoke.jpg");
    void                   DeleteEffect(class ParticleEffect* effect);
    void                   Update(const float deltaTime);
    void                   UpdateEditorLifeCycle();
    void                   RefreshEditor();
    class ParticleEffect*  GetCurrentEditorEffect() { return _editorCurrentEffect; }
    void                   TurnOffEditorMode();


    UMPARTICLE_PROPERTY(bool, _isAutoRefresh, AutoRefresh, false);

    UINT                  GetMaxCount();
    UINT                  GetTotalCount(std::string_view sceneName);
    std::vector<Texture*> GetActiveAlbedos(std::string_view sceneName);
    ID3D12Resource*       GetComputeOutputResource(std::string_view sceneName);

    UINT                                  GetRibbonCount(std::string_view sceneName);
    std::vector<std::vector<ribbonIndex>> GetRibbonEmitterIndices(std::string_view sceneName);
    std::vector<Texture*>                 GetActiveRibbonAlbedos(std::string_view sceneName);
    ID3D12Resource*                       GetRibbonOutputResource(std::string_view sceneName);
    UINT64                                GetComputeFenceValue(std::string_view sceneName);


public:
    void SetCamera(std::string_view viewName);
    void SetCamera(std::shared_ptr<Camera> camera);
    void SetCurrentEditorEffect(class ParticleEffect* newEffect);


    void AddSceneResource(std::string_view sceneName);
    void AddSceneResource(std::string_view sceneName, std::string_view sharedFrom);

private:

    void InitializeComputeCommandObject();
    void InitializeComputeCommandObject(std::string_view sceneName);
    void InitializeParticleComputeShader();
    void InitializeParticleComputeRootSignature();
    void InitializeParticleComputePSO();

    void InitializeDescriptorHeap();

    //void CreateParticleResources();
    void CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, ComPtr<ID3D12Resource>& uploadResource,
                                UINT bufferSize, UINT stride);
    void CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride);
    void CreateUAVBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride);
    void CreateConstantBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize);

    void AwakeParticles(float deltaTime, ParticleUpdateResource* targetScene);
    void DispatchSprite(float deltaTime, std::string sceneName);
    void DispatchRibbon(float deltaTime, std::string sceneName);
    void UpdateAndCopyParticleResource(float deltaTime, ParticleUpdateResource* scene);
    void UpdateMvpConstant(float deltaTime, ParticleRenderResource* scene);
    void CopyParticleResourece(float deltaTime, ParticleUpdateResource* scene);
    void                                    UpdateLifeCycle(float deltaTime);
    std::unordered_map<std::string, UINT64> _computeFences;
    std::unordered_map<std::string, ParticleSceneResource> _sceneResources;
    std::unordered_set<ParticleUpdateResource*>            _particleUpdateResources;

    
    
    std::shared_ptr<Camera> _camera;
    float                   lastFrameTime = 0;


    ComPtr<ID3D12CommandAllocator>    _computeAllocator;
    ComPtr<ID3D12GraphicsCommandList> _computeCommandList;

    ComPtr<ID3D12RootSignature> _computeSpriteRootSignature;
    ComPtr<ID3D12PipelineState> _computeSpritePSO;
    ComPtr<ID3DBlob>            _computeSpriteShaderBlob;

    ComPtr<ID3D12RootSignature> _computeRibbonRootSignature;
    ComPtr<ID3D12PipelineState> _computeRibbonPSO;
    ComPtr<ID3DBlob>            _computeRibbonShaderBlob;


    ComPtr<ID3D12DescriptorHeap> _cbvSrvUavHeap;
    UINT                         _descriptorSize;


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
    
 

    std::vector<std::vector<ribbonIndex>> _ribbonIndices;
    std::vector<std::vector<ribbonIndex>> _ribbonEditorIndices;



    UINT _totalCount = 0;
    UINT _editorCount = 0;

    UINT _ribbonTotalCount = 0;
    UINT _ribbonEditorCount = 0;


    float _elapsedTimer = 0.f;
    int   nameingIndex  = 0;


   UMPARTICLE_PROPERTY(float, _deltaScale, DeltaScale, 1.f);


};
