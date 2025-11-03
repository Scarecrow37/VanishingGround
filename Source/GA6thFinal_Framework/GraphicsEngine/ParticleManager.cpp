#include "pch.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"
#include "ParticleManager.h"

ComputeFX<GE::CS::COMPUTE_SPRITE> computeSpriteFX;
ComputeFX<GE::CS::COMPUTE_RIBBON> computeRibbonFX;
// =================================================================================================================
// [ 1. Constructor / Destructor & Initialize ]
// =================================================================================================================

ParticleManager::ParticleManager() = default;

ParticleManager::~ParticleManager() = default;

void ParticleManager::Initialize(UINT maxParticles)
{
    _maxParticles = maxParticles;
    InitializeComputeCommandObject();
    InitializeParticleComputePSO();
}

// =================================================================================================================
// [ 2. Scene Management ]
// =================================================================================================================

void ParticleManager::AddSceneResource(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    if (_sceneResources.count(sName))
    {
        return; // Scene already exists
    }

    std::wstring          wSceneName(sceneName.begin(), sceneName.end());
    ParticleSceneResource newSceneResource;
    newSceneResource.Name = sName;

    // Special case for "Editor" scene to share "Game" scene's update resource
    if (sName == "Editor" && _sceneResources.count("Game"))
    {
        UINT particleOutputSize = _maxParticles * sizeof(ParticleOutput);
        UINT mvpConstantSize    = sizeof(MVPConstants);

        // Share the update resource from the "Game" scene
        newSceneResource.UpdateParticleResource = _sceneResources.at("Game").UpdateParticleResource;

        // Create a new unique render resource for the "Editor"
        newSceneResource.RenderParticleResource       = std::make_unique<ParticleRenderResource>();
        newSceneResource.RenderParticleResource->Name = sName;

        {
            CreateUAVBuffer(newSceneResource.RenderParticleResource->SimulationOutput, particleOutputSize,
                            sizeof(ParticleOutput));
            newSceneResource.RenderParticleResource->SimulationOutput->SetName((wSceneName + L" output").c_str());

            CreateUAVBuffer(newSceneResource.RenderParticleResource->RibbonSimulationOutput, particleOutputSize,
                            sizeof(ParticleOutput));
            newSceneResource.RenderParticleResource->RibbonSimulationOutput->SetName(
                (wSceneName + L" ribbon output").c_str());

            CreateConstantBuffer(newSceneResource.RenderParticleResource->MvpConstant, mvpConstantSize);
            newSceneResource.RenderParticleResource->MvpConstant->SetName((wSceneName + L" mvp constants").c_str());
        }

        InitializeComputeCommandObject(newSceneResource);
        _sceneResources.emplace(sName, std::move(newSceneResource));
        return;
    }

    // Default behavior for all other scenes (or "Editor" if "Game" doesn't exist yet)
    UINT particleInputSize  = _maxParticles * sizeof(Particle);
    UINT emitterInfoSize    = _maxEmitters * sizeof(EmitterInfo);
    UINT particleOutputSize = _maxParticles * sizeof(ParticleOutput);
    UINT mvpConstantSize    = sizeof(MVPConstants);

    newSceneResource.UpdateParticleResource       = std::make_shared<ParticleUpdateResource>();
    newSceneResource.UpdateParticleResource->Name = sName;

    newSceneResource.RenderParticleResource       = std::make_unique<ParticleRenderResource>();
    newSceneResource.RenderParticleResource->Name = sName;

    // Create GPU resources
    {
        // Input buffers (for UpdateResource)
        {
            CreateStructuredBuffer(newSceneResource.UpdateParticleResource->ParticleInput,
                                   newSceneResource.UpdateParticleResource->ParticleInputUpload, particleInputSize,
                                   sizeof(Particle));
            newSceneResource.UpdateParticleResource->ParticleInput->SetName((wSceneName + L" particle input").c_str());
            newSceneResource.UpdateParticleResource->ParticleInputUpload->SetName(
                (wSceneName + L" particle input upload").c_str());

            CreateStructuredBuffer(newSceneResource.UpdateParticleResource->RibbonParticleInput,
                                   newSceneResource.UpdateParticleResource->RibbonParticleInputUpload,
                                   particleInputSize, sizeof(Particle));
            newSceneResource.UpdateParticleResource->RibbonParticleInput->SetName(
                (wSceneName + L" ribbon particle input").c_str());
            newSceneResource.UpdateParticleResource->RibbonParticleInputUpload->SetName(
                (wSceneName + L" ribbon particle input upload").c_str());
        }
        // Emitter info buffers (for UpdateResource)
        {
            CreateStructuredBuffer(newSceneResource.UpdateParticleResource->EmitterInfo,
                                   newSceneResource.UpdateParticleResource->EmitterInfoUpload, emitterInfoSize,
                                   sizeof(EmitterInfo));
            newSceneResource.UpdateParticleResource->EmitterInfo->SetName((wSceneName + L" emitter info").c_str());
            newSceneResource.UpdateParticleResource->EmitterInfoUpload->SetName(
                (wSceneName + L" emitter info upload").c_str());

            CreateStructuredBuffer(newSceneResource.UpdateParticleResource->RibbonEmitterInfo,
                                   newSceneResource.UpdateParticleResource->RibbonEmitterInfoUpload, emitterInfoSize,
                                   sizeof(EmitterInfo));
            newSceneResource.UpdateParticleResource->RibbonEmitterInfo->SetName(
                (wSceneName + L" ribbon emitter info").c_str());
            newSceneResource.UpdateParticleResource->RibbonEmitterInfoUpload->SetName(
                (wSceneName + L" ribbon emitter info upload").c_str());
        }
        // Output buffers (for RenderResource)
        {
            CreateUAVBuffer(newSceneResource.RenderParticleResource->SimulationOutput, particleOutputSize,
                            sizeof(ParticleOutput));
            newSceneResource.RenderParticleResource->SimulationOutput->SetName((wSceneName + L" output").c_str());

            CreateUAVBuffer(newSceneResource.RenderParticleResource->RibbonSimulationOutput, particleOutputSize,
                            sizeof(ParticleOutput));
            newSceneResource.RenderParticleResource->RibbonSimulationOutput->SetName(
                (wSceneName + L" ribbon output").c_str());
        }
        // MVP constant buffer (for RenderResource)
        {
            CreateConstantBuffer(newSceneResource.RenderParticleResource->MvpConstant, mvpConstantSize);
            newSceneResource.RenderParticleResource->MvpConstant->SetName((wSceneName + L" mvp constants").c_str());
        }
    }

    InitializeComputeCommandObject(newSceneResource);
    newSceneResource.UpdateParticleResource->TotalParticles.resize(_maxParticles);
    newSceneResource.UpdateParticleResource->RibbonTotalParticles.resize(_maxParticles);

    _sceneResources.emplace(sName, std::move(newSceneResource));
}

// =================================================================================================================
// [ 3. Effect Lifecycle Management ]
// =================================================================================================================

ParticleEffect* ParticleManager::RegisterEffect(EffectID id, const std::string& keyString, std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    if (!_sceneResources.count(sName))
        return nullptr;

    auto        newEffect = std::make_shared<ParticleEffect>();
    std::string name      = "Effect" + std::to_string(_namingIndex++);
    newEffect->SetEffectName(name);

    ParticleEffect* rawPtr = newEffect.get();

    _sceneResources.at(sName).UpdateParticleResource->SceneEffects.push_back(newEffect);

    _effectIDTable[id][keyString] = newEffect;
    return rawPtr;
}

void ParticleManager::DeleteEffect(EffectID id, const std::string& keyString, const std::string& sceneName)
{
    auto effectIDIter = _effectIDTable.find(id);
    if (effectIDIter == _effectIDTable.end())
        return;

    auto& effectMap        = effectIDIter->second;
    auto  targetEffectIter = effectMap.find(keyString);
    if (targetEffectIter == effectMap.end())
        return;

    if (auto sp = targetEffectIter->second.lock())
    {
        sp->SetRemoveFlag(true);
    }

    effectMap.erase(targetEffectIter);
    if (effectMap.empty())
    {
        _effectIDTable.erase(effectIDIter);
    }

    // Clean up bone matrix cache
    if (auto boneMatIter = _effectBoneMatTable.find(id); boneMatIter != _effectBoneMatTable.end())
    {
        auto& boneMatMap = boneMatIter->second;
        boneMatMap.erase(keyString);
        if (boneMatMap.empty())
        {
            _effectBoneMatTable.erase(boneMatIter);
        }
    }
}

ParticleEffect* ParticleManager::FindEffect(EffectID id, const std::string& keyString)
{
    if (auto effectIDIter = _effectIDTable.find(id); effectIDIter != _effectIDTable.end())
    {
        auto& effectMap = effectIDIter->second;
        if (auto targetEffectIter = effectMap.find(keyString); targetEffectIter != effectMap.end())
        {
            if (auto sp = targetEffectIter->second.lock())
            {
                return sp.get();
            }
            effectMap.erase(targetEffectIter);
            if (effectMap.empty())
            {
                _effectIDTable.erase(effectIDIter);
            }

            // Clean up bone matrix cache for expired effect
            if (auto boneMatIter = _effectBoneMatTable.find(id); boneMatIter != _effectBoneMatTable.end())
            {
                auto& boneMatMap = boneMatIter->second;
                boneMatMap.erase(keyString);
                if (boneMatMap.empty())
                {
                    _effectBoneMatTable.erase(boneMatIter);
                }
            }
            return nullptr;
        }
    }
    return nullptr;
}

void ParticleManager::PlayEffect(EffectID id, const std::string& keyString, EffectCallback callback)
{
    if (auto effect = FindEffect(id, keyString))
    {
        if (effect->GetBoneFollowFlag() && effect->GetBoneMatrix() == nullptr)
        {
            if (auto boneMatIter = _effectBoneMatTable.find(id); boneMatIter != _effectBoneMatTable.end())
            {
                auto& boneMatMap = boneMatIter->second;
                if (auto targetBoneMatIter = boneMatMap.find(keyString); targetBoneMatIter != boneMatMap.end())
                {
                    effect->SetBoneMatrix(targetBoneMatIter->second);
                }
            }
        }
        effect->Play(callback);
    }
}

void ParticleManager::PlayEffect(EffectID id, const std::string& keyString) 
{
    if (auto effect = FindEffect(id, keyString))
    {
        if (effect->GetBoneFollowFlag() && effect->GetBoneMatrix() == nullptr)
        {
            if (auto boneMatIter = _effectBoneMatTable.find(id); boneMatIter != _effectBoneMatTable.end())
            {
                auto& boneMatMap = boneMatIter->second;
                if (auto targetBoneMatIter = boneMatMap.find(keyString); targetBoneMatIter != boneMatMap.end())
                {
                    effect->SetBoneMatrix(targetBoneMatIter->second);
                }
            }
        }
        effect->Play();
    }
}

void ParticleManager::StopEffect(EffectID id, const std::string& keyString)
{
    if (auto effect = FindEffect(id, keyString))
        effect->Stop();
}

// =================================================================================================================
// [ 4. Emitter Management ]
// =================================================================================================================

ParticleEmitter* ParticleManager::RegisterEmitter(ParticleEffect* effect, SIZE_T maxParticles, float emissionRate,
                                                  float emitterLifetime, LocationShape locatorShape,
                                                  Vector3 locationFactor, ParticleType particleType,
                                                  const std::wstring& meshspritePath)
{
    if (!effect)
        return nullptr;
    return effect->AddEmitter(maxParticles, emissionRate, emitterLifetime, locatorShape, locationFactor, particleType,
                              meshspritePath);
}

// =================================================================================================================
// [ 5. Main Update Loop ]
// =================================================================================================================

void ParticleManager::Update(const float deltaTime)
{
    float delta = deltaTime * _deltaScale;

    // 1. Gather unique update resources
    std::set<std::shared_ptr<ParticleUpdateResource>> uniqueUpdateResources;
    for (const auto& pair : _sceneResources)
    {
        if (pair.second.UpdateParticleResource)
        {
            uniqueUpdateResources.insert(pair.second.UpdateParticleResource);
        }
    }

    // 2. Update effects, awake particles, and copy to GPU
    _computeAllocator->Reset();
    _computeCommandList->Reset(_computeAllocator.Get(), nullptr);
    for (const auto& updateResource : uniqueUpdateResources)
    {
        for (const auto& effect : updateResource->SceneEffects)
        {
            if (effect->GetActiveFlag())
            {
                effect->Update(delta);
            }
        }
        AwakeParticles(deltaTime, updateResource);
        UpdateAndCopyParticleResource(deltaTime, updateResource);
    }
    _computeCommandList->Close();
    Global::commandController->ExecuteCommand(COMPUTE_QUEUE, _computeCommandList.Get());

    // 3. Dispatch compute shaders for each scene
    for (auto& pair : _sceneResources)
    {
        ParticleSceneResource& scene = pair.second;
        if (scene.UpdateParticleResource && !scene.UpdateParticleResource->SceneEffects.empty())
        {
            scene.CommandAllocator->Reset();
            scene.CommandList->Reset(scene.CommandAllocator.Get(), nullptr);

            UpdateMvpConstant(deltaTime, scene.RenderParticleResource.get());

            scene.CommandList->SetPipelineState(_computeSpritePSO.Get());
            scene.CommandList->SetComputeRootSignature(computeSpriteFX.GetRootSignature());
            DispatchSprite(deltaTime, scene.Name);

            scene.CommandList->SetPipelineState(_computeRibbonPSO.Get());
            scene.CommandList->SetComputeRootSignature(computeRibbonFX.GetRootSignature());
            DispatchRibbon(deltaTime, scene.Name);

            scene.CommandList->Close();
            Global::commandController->ExecuteCommand(COMPUTE_QUEUE, scene.CommandList.Get());

            _computeFences[scene.Name] = Global::commandController->SignalCommandQueue(COMPUTE_QUEUE);
        }
    }

    UpdateLifeCycle(delta);

    if (auto sp = _editorCurrentEffect.lock())
    {
        if (sp->GetActiveFlag())
        {
            sp->UpdateParticleLifeCycle(delta);
        }
        UpdateEditorLifeCycle(); // 이 함수도 내부에서 lock() 사용하도록 수정되어 있어야 안전

        if (!sp->GetActiveFlag() && _isAutoRefresh)
        {
            RefreshEditor();
        }
    }

}

// =================================================================================================================
// [ 6. Getters ]
// =================================================================================================================

UINT ParticleManager::GetMaxCount()
{
    return _maxParticles;
}

UINT ParticleManager::GetTotalCount(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    auto it    = _sceneResources.find(sName);
    if (it != _sceneResources.end())
    {
        return it->second.UpdateParticleResource->TotalCount;
    }
    return 0;
}

const std::vector<UINT>& ParticleManager::GetActiveAlbedos(std::string_view sceneName)
{
    static const std::vector<UINT> empty;
    auto                           sName = std::string(sceneName);
    auto                           it    = _sceneResources.find(sName);
    if (it != _sceneResources.end())
    {
        return it->second.UpdateParticleResource->ActiveEmitterAlbedos;
    }
    return empty;
}

ID3D12Resource* ParticleManager::GetComputeOutputResource(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    auto it    = _sceneResources.find(sName);
    if (it != _sceneResources.end())
    {
        return it->second.RenderParticleResource->SimulationOutput.Get();
    }
    return nullptr;
}

UINT ParticleManager::GetRibbonCount(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    auto it    = _sceneResources.find(sName);
    if (it != _sceneResources.end())
    {
        return it->second.UpdateParticleResource->RibbonTotalCount;
    }
    return 0;
}

const std::vector<std::vector<RibbonIndex>>& ParticleManager::GetRibbonEmitterIndices(std::string_view sceneName)
{
    static const std::vector<std::vector<RibbonIndex>> empty;
    auto                                               sName = std::string(sceneName);
    auto                                               it    = _sceneResources.find(sName);
    if (it != _sceneResources.end())
    {
        return it->second.UpdateParticleResource->RibbonIndices;
    }
    return empty;
}

const std::vector<UINT>& ParticleManager::GetActiveRibbonAlbedos(std::string_view sceneName)
{
    static const std::vector<UINT> empty;
    auto                           sName = std::string(sceneName);
    auto                           it    = _sceneResources.find(sName);
    if (it != _sceneResources.end())
    {
        return it->second.UpdateParticleResource->RibbonActiveEmitterAlbedos;
    }
    return empty;
}

ID3D12Resource* ParticleManager::GetRibbonOutputResource(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    auto it    = _sceneResources.find(sName);
    if (it != _sceneResources.end())
    {
        return it->second.RenderParticleResource->RibbonSimulationOutput.Get();
    }
    return nullptr;
}

UINT64 ParticleManager::GetComputeFenceValue(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    auto it    = _computeFences.find(sName);
    if (it != _computeFences.end())
    {
        return it->second;
    }
    return 0;
}

// =================================================================================================================
// [ 7. Setters ]
// =================================================================================================================

void ParticleManager::SetActiveFlag(EffectID id, const std::string& keyString, bool flag)
{
    if (auto effect = FindEffect(id, keyString))
        effect->SetActiveFlag(flag);
}

void ParticleManager::SetRemoveFlag(EffectID id, const std::string& keyString, bool flag)
{
    if (auto effect = FindEffect(id, keyString))
        effect->SetRemoveFlag(flag);
}

void ParticleManager::SetFollowBoneFlag(EffectID id, const std::string& keyString, bool* flag)
{
    if (auto effect = FindEffect(id, keyString))
        effect->SetBoneFollowFlag(flag);
}

void ParticleManager::SetBoneMatrix(EffectID id, const std::string& keyString, const Matrix* boneMatrix)
{
    if (auto effect = FindEffect(id, keyString))
        effect->SetBoneMatrix(boneMatrix);
    _effectBoneMatTable[id][keyString] = boneMatrix;
}

// =================================================================================================================
// [ 8. Editor-specific ]
// =================================================================================================================

ParticleEffect* ParticleManager::RegisterEffectOnEditor()
{
    if (!_sceneResources.count("ParticleEditor"))
        return nullptr;

    auto& editorEffects = _sceneResources.at("ParticleEditor").UpdateParticleResource->SceneEffects;

    editorEffects.clear();

    auto        newEffect = std::make_shared<ParticleEffect>();
    std::string name      = "Effect" + std::to_string(_namingIndex++);
    newEffect->SetEffectName(name);

    // weak_ptr로만 현재 이펙트 보관
    _editorCurrentEffect = newEffect;

    editorEffects.push_back(newEffect);
    return newEffect.get();
}

void ParticleManager::SetCurrentEditorEffect(class ParticleEffect* newEffect)
{
    if (_sceneResources.count("ParticleEditor"))
    {
        auto&                         vec = _sceneResources.at("ParticleEditor").UpdateParticleResource->SceneEffects;
        std::weak_ptr<ParticleEffect> found;
        for (const auto& sp : vec)
        {
            if (sp.get() == newEffect)
            {
                found = sp;
                break;
            }
        }
        _editorCurrentEffect = found;
    }
    else
    {
        _editorCurrentEffect.reset();
    }
    RefreshEditor();
}

class ParticleEffect* ParticleManager::GetCurrentEditorEffect()
{
    if (auto sp = _editorCurrentEffect.lock())
        return sp.get();
    return nullptr;
}

void ParticleManager::RefreshEditor()
{
    _editorRefreshFlag = true;
}

void ParticleManager::UpdateEditorLifeCycle()
{
    auto sp = _editorCurrentEffect.lock();
    if (!sp)
        return;

    if (sp->GetPlayFlag())
    {
        sp->SetPlayFlag(false);
        sp->Reset();
    }
}

// =================================================================================================================
// [ 9. Other Public Methods ]
// =================================================================================================================

void ParticleManager::ChangeTexture()
{
    std::set<std::shared_ptr<ParticleUpdateResource>> uniqueUpdateResources;
    for (const auto& pair : _sceneResources)
    {
        if (pair.second.UpdateParticleResource)
        {
            uniqueUpdateResources.insert(pair.second.UpdateParticleResource);
        }
    }

    for (const auto& updateResource : uniqueUpdateResources)
    {
        for (const auto& effect : updateResource->SceneEffects)
        {
            if (effect->GetActiveFlag())
            {
                for (const auto& emitter : effect->GetEmitterList())
                {
                    emitter->FlushTextureResource();
                }
            }
        }
    }

    if (_editorRefreshFlag)
    {
        RefreshCurrentEditorEffect();
    }
}

// =================================================================================================================
// [ 10. Internal Processing & DX12 Resource Management ]
// =================================================================================================================
void ParticleManager::InitializeComputeCommandObject(ParticleSceneResource& scene)
{

    D3D12_COMMAND_QUEUE_DESC desc{
        .Type     = D3D12_COMMAND_LIST_TYPE_COMPUTE,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
    };

    HRESULT hr = S_OK;
    hr         = Global::device->GetDevice()->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(&scene.CommandAllocator));
    FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeComputeCommandObject CreateCommandAllocator Failed");

    hr = Global::device->GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, scene.CommandAllocator.Get(), nullptr,
                                                        IID_PPV_ARGS(scene.CommandList.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeComputeCommandObject CreateCommandList Failed");
    scene.CommandList->Close();
}

void ParticleManager::InitializeComputeCommandObject()
{
    D3D12_COMMAND_QUEUE_DESC desc{
        .Type     = D3D12_COMMAND_LIST_TYPE_COMPUTE,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
    };

    HRESULT hr = S_OK;
    hr         = Global::device->GetDevice()->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(&_computeAllocator));
    FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeComputeCommandObject CreateCommandAllocator Failed");

    hr = Global::device->GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, _computeAllocator.Get(), nullptr,
                                                        IID_PPV_ARGS(_computeCommandList.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeComputeCommandObject CreateCommandList Failed");
    _computeCommandList->Close();
}

void ParticleManager::InitializeParticleComputePSO()
{
    // initialize sprite pipeline state object
    {
        ComputePipelineStateStream pss;
        computeSpriteFX.SetPipelineStateStream(pss);
        _computeSpritePSO = Global::pipelineStateManager->GetPipelineState(pss);
    }
    // initialize ribbon pipeline state object
    {
        ComputePipelineStateStream pss;
        computeRibbonFX.SetPipelineStateStream(pss);
        _computeRibbonPSO = Global::pipelineStateManager->GetPipelineState(pss);
    }
}

void ParticleManager::CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, ComPtr<ID3D12Resource>& uploadResource,
                                             UINT bufferSize, UINT stride)
{
    D3D12_RESOURCE_DESC bufferDesc      = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    auto                defaultProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    FAILED_CHECK_MESSAGE(Global::device->GetDevice()->CreateCommittedResource(&defaultProperty, D3D12_HEAP_FLAG_NONE,
                                                                              &bufferDesc, D3D12_RESOURCE_STATE_COMMON,
                                                                              nullptr, IID_PPV_ARGS(&resource)),
                         L"CreateCommittedResource for default buffer Failed");

    auto uploadProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    FAILED_CHECK_MESSAGE(Global::device->GetDevice()->CreateCommittedResource(
                             &uploadProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                             nullptr, IID_PPV_ARGS(&uploadResource)),
                         L"CreateCommittedResource for upload buffer Failed");
}

void ParticleManager::CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride)
{
    D3D12_RESOURCE_DESC bufferDesc      = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    auto                defaultProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    FAILED_CHECK_MESSAGE(Global::device->GetDevice()->CreateCommittedResource(&defaultProperty, D3D12_HEAP_FLAG_NONE,
                                                                              &bufferDesc, D3D12_RESOURCE_STATE_COMMON,
                                                                              nullptr, IID_PPV_ARGS(&resource)),
                         L"CreateCommittedResource for structured buffer Failed");
}

void ParticleManager::CreateUAVBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride)
{
    D3D12_RESOURCE_DESC bufferDesc =
        CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    auto defaultProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    FAILED_CHECK_MESSAGE(Global::device->GetDevice()->CreateCommittedResource(&defaultProperty, D3D12_HEAP_FLAG_NONE,
                                                                              &bufferDesc, D3D12_RESOURCE_STATE_COMMON,
                                                                              nullptr, IID_PPV_ARGS(&resource)),
                         L"CreateCommittedResource for UAV buffer Failed");
}

void ParticleManager::CreateConstantBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize)
{
    UINT                alignedBufferSize = (bufferSize + 255) & ~255;
    D3D12_RESOURCE_DESC bufferDesc        = CD3DX12_RESOURCE_DESC::Buffer(alignedBufferSize);
    auto                uploadProperty    = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    FAILED_CHECK_MESSAGE(Global::device->GetDevice()->CreateCommittedResource(
                             &uploadProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                             nullptr, IID_PPV_ARGS(&resource)),
                         L"CreateCommittedResource for constant buffer Failed");
}

void ParticleManager::AwakeParticles(float deltaTime, const std::shared_ptr<ParticleUpdateResource>& scene)
{
    scene->EmitterMatrix.clear();
    scene->ActiveEmitterAlbedos.clear();
    scene->RibbonEmitterMatrix.clear();
    scene->RibbonActiveEmitterAlbedos.clear();
    scene->RibbonIndices.clear();

    UINT totalSpriteParticles = 0;
    UINT totalRibbonParticles = 0;
    for (const auto& effect : scene->SceneEffects)
    {
        if (effect->GetActiveFlag())
        {
            for (const auto& emitter : effect->GetEmitterList())
            {
                if (emitter->GetActiveFlag())
                {
                    if (emitter->_particleType == ParticleType::SPRITE)
                    {
                        totalSpriteParticles += emitter->GetActiveParticleCount();
                    }
                    else if (emitter->_particleType == ParticleType::RIBBON)
                    {
                        totalRibbonParticles += emitter->GetActiveParticleCount() * 2;
                    }
                }
            }
        }
    }

    scene->TotalParticles.reserve(totalSpriteParticles);
    scene->TotalParticles.clear();
    scene->RibbonTotalParticles.reserve(totalRibbonParticles);
    scene->RibbonTotalParticles.clear();

    UINT emitterIndex        = 0;
    UINT ribbonEmitterIndex  = 0;
    UINT ribbonparticleIndex = 0;
    
    const auto& toneMappingProperty = std::any_cast<const ToneMappingProperty&>(Global::renderPassDatas->GetRenderPassProperty("ToneMappingPass"));
    float alphaFactor = std::max(toneMappingProperty.Exposure, 0.01f);

    for (const auto& effect : scene->SceneEffects)
    {
        if (effect->GetActiveFlag())
        {
            for (const auto& emitter : effect->GetEmitterList())
            {
                if (emitter->GetActiveFlag())
                {
                    
                    if (ParticleType::SPRITE == emitter->_particleType)
                    {
                        auto spriteModule = emitter->_particleRenderModule->AsSprite();
                        scene->ActiveEmitterAlbedos.push_back(spriteModule->GetAlbedoTextureID());
                        Matrix worldMatrix =
                            emitter->GetUseWorldSpace() ? Matrix::Identity : emitter->GetWorldMatrix().Transpose();
                        Matrix orientMatrix = emitter->GetWorldMatrix().Transpose();
                        scene->EmitterMatrix.push_back(
                            {worldMatrix, orientMatrix, emitter->GetDragPoint(), emitter->GetDragForce(),
                             emitter->GetVortexForce(), emitter->GetStartScale(), emitter->GetEndScale(),
                             Vector4(emitter->GetStartColor().x / alphaFactor, emitter->GetStartColor().y / alphaFactor,
                                     emitter->GetStartColor().z / alphaFactor,
                                     emitter->GetStartOpacity() ),
                             Vector4(emitter->GetEndColor().x / alphaFactor, emitter->GetEndColor().y / alphaFactor,
                                     emitter->GetEndColor().z / alphaFactor,
                                     emitter->GetEndOpacity()),
                             Vector4(emitter->GetParticleLifetime(), emitter->GetUseWorldSpace() ? 1.0f : 0.0f, 0, 0),
                             Vector4(0, 0, 0, 0), Vector4(0, 0, 0, 0), Vector4(0, 0, 0, 0)});

                        const auto& particlePool = emitter->GetParticlePool();
                        UINT        activeCount  = emitter->GetActiveParticleCount();

                        if (activeCount > 0)
                        {
                            size_t insertPosition = scene->TotalParticles.size();
                            scene->TotalParticles.insert(scene->TotalParticles.end(), particlePool.begin(),
                                                         particlePool.begin() + activeCount);
                            for (size_t i = 0; i < activeCount; ++i)
                            {
                                scene->TotalParticles[insertPosition + i].SetEmitterIndex(emitterIndex);
                            }
                        }
                        emitterIndex++;
                    }
                    else if (ParticleType::RIBBON == emitter->_particleType)
                    {
                        auto ribbonModule = emitter->_particleRenderModule->AsRibbon();
                        scene->RibbonActiveEmitterAlbedos.push_back(ribbonModule->GetAlbedoTextureID());
                        Matrix worldMatrix =
                            emitter->GetUseWorldSpace() ? Matrix::Identity : emitter->GetWorldMatrix().Transpose();

                        Matrix orientMatrix = emitter->GetWorldMatrix().Transpose();
                        scene->RibbonEmitterMatrix.push_back(
                            {worldMatrix, orientMatrix, emitter->GetDragPoint(), emitter->GetDragForce(),
                             emitter->GetVortexForce(), emitter->GetStartScale(), emitter->GetEndScale(),
                             Vector4(emitter->GetStartColor().x / alphaFactor, emitter->GetStartColor().y / alphaFactor,
                                     emitter->GetStartColor().z / alphaFactor,
                                     emitter->GetStartOpacity() ),
                             Vector4(emitter->GetEndColor().x / alphaFactor, emitter->GetEndColor().y / alphaFactor,
                                     emitter->GetEndColor().z / alphaFactor,
                                     emitter->GetEndOpacity() ),
                             Vector4(emitter->GetParticleLifetime(), 0, 0, 0), ribbonModule->GetStartNormal(),
                             ribbonModule->GetEndNormal(), ribbonModule->GetRibbonVector()});

                        const auto& particlePool = emitter->GetParticlePool();
                        UINT        activeCount  = emitter->GetActiveParticleCount();

                        if (activeCount > 0)
                        {
                            std::vector<RibbonIndex> emitterIndices;
                            emitterIndices.reserve(activeCount * 2);
                            float lifetime = emitter->GetParticleLifetime();

                            for (UINT i = 0; i < activeCount; ++i)
                            {
                                Particle particle = particlePool[i];
                                particle.SetEmitterIndex(ribbonEmitterIndex);
                                scene->RibbonTotalParticles.push_back(particle);
                                emitterIndices.push_back({ribbonparticleIndex++, particle.GetAge() / lifetime});
                                emitterIndices.push_back({ribbonparticleIndex++, particle.GetAge() / lifetime});
                            }
                            std::sort(
                                emitterIndices.begin(), emitterIndices.end(),
                                [](const RibbonIndex& a, const RibbonIndex& b) -> bool { return a.Ratio < b.Ratio; });

                            if (!emitterIndices.empty())
                            {
                                scene->RibbonIndices.push_back(std::move(emitterIndices));
                            }
                        }
                        ribbonEmitterIndex++;
                    }
                }
            }
        }
    }
    scene->TotalCount       = (UINT)scene->TotalParticles.size();
    scene->RibbonTotalCount = (UINT)(scene->RibbonTotalParticles.size() * 2);
}

void ParticleManager::UpdateAndCopyParticleResource(float deltaTime, const std::shared_ptr<ParticleUpdateResource>& scene)
{
    // memcpy to upload heap
    {
        void* mappedData = nullptr;
        if (scene->TotalCount > 0)
        {
            scene->ParticleInputUpload->Map(0, nullptr, &mappedData);
            memcpy(mappedData, scene->TotalParticles.data(), scene->TotalCount * sizeof(Particle));
            scene->ParticleInputUpload->Unmap(0, nullptr);

            scene->EmitterInfoUpload->Map(0, nullptr, &mappedData);
            memcpy(mappedData, scene->EmitterMatrix.data(), scene->EmitterMatrix.size() * sizeof(EmitterInfo));
            scene->EmitterInfoUpload->Unmap(0, nullptr);
        }
        if (scene->RibbonTotalCount > 0)
        {
            scene->RibbonParticleInputUpload->Map(0, nullptr, &mappedData);
            memcpy(mappedData, scene->RibbonTotalParticles.data(),
                   scene->RibbonTotalParticles.size() * sizeof(Particle));
            scene->RibbonParticleInputUpload->Unmap(0, nullptr);

            scene->RibbonEmitterInfoUpload->Map(0, nullptr, &mappedData);
            memcpy(mappedData, scene->RibbonEmitterMatrix.data(),
                   scene->RibbonEmitterMatrix.size() * sizeof(EmitterInfo));
            scene->RibbonEmitterInfoUpload->Unmap(0, nullptr);
        }
    }

    // copy data from upload to default heap
    {
        CD3DX12_RESOURCE_BARRIER preCopyBarriers[] = {
            CD3DX12_RESOURCE_BARRIER::Transition(scene->ParticleInput.Get(), D3D12_RESOURCE_STATE_COMMON,
                                                 D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->EmitterInfo.Get(), D3D12_RESOURCE_STATE_COMMON,
                                                 D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->RibbonParticleInput.Get(), D3D12_RESOURCE_STATE_COMMON,
                                                 D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->RibbonEmitterInfo.Get(), D3D12_RESOURCE_STATE_COMMON,
                                                 D3D12_RESOURCE_STATE_COPY_DEST)};
        _computeCommandList->ResourceBarrier(_countof(preCopyBarriers), preCopyBarriers);

        if (scene->TotalCount > 0)
        {
            UINT64 particleDataSize = scene->TotalParticles.size() * sizeof(Particle);
            _computeCommandList->CopyBufferRegion(scene->ParticleInput.Get(), 0, scene->ParticleInputUpload.Get(), 0,
                                                  particleDataSize);

            UINT64 emitterDataSize = scene->EmitterMatrix.size() * sizeof(EmitterInfo);
            _computeCommandList->CopyBufferRegion(scene->EmitterInfo.Get(), 0, scene->EmitterInfoUpload.Get(), 0,
                                                  emitterDataSize);
        }

        if (scene->RibbonTotalCount > 0)
        {
            UINT64 particleDataSize = scene->RibbonTotalParticles.size() * sizeof(Particle);
            _computeCommandList->CopyBufferRegion(scene->RibbonParticleInput.Get(), 0,
                                                  scene->RibbonParticleInputUpload.Get(), 0, particleDataSize);

            UINT64 emitterDataSize = scene->RibbonEmitterMatrix.size() * sizeof(EmitterInfo);
            _computeCommandList->CopyBufferRegion(scene->RibbonEmitterInfo.Get(), 0,
                                                  scene->RibbonEmitterInfoUpload.Get(), 0, emitterDataSize);
        }

        CD3DX12_RESOURCE_BARRIER postCopyBarriers[] = {
            CD3DX12_RESOURCE_BARRIER::Transition(scene->ParticleInput.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                 D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->EmitterInfo.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                 D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->RibbonParticleInput.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                 D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->RibbonEmitterInfo.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                 D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)};
        _computeCommandList->ResourceBarrier(_countof(postCopyBarriers), postCopyBarriers);
    }
}

void ParticleManager::DispatchSprite(float deltaTime, std::string sceneName)
{
    ParticleSceneResource& scene = _sceneResources.at(sceneName);
    if (scene.UpdateParticleResource->TotalCount == 0)
        return;

    CD3DX12_RESOURCE_BARRIER computeOutputBarrier =
        CD3DX12_RESOURCE_BARRIER::Transition(scene.RenderParticleResource->SimulationOutput.Get(),
                                             D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    scene.CommandList->ResourceBarrier(1, &computeOutputBarrier);

    scene.CommandList->SetComputeRootConstantBufferView(
        computeSpriteFX.GetRootParameterIndex("mvp"),
        scene.RenderParticleResource->MvpConstant->GetGPUVirtualAddress());
    scene.CommandList->SetComputeRootShaderResourceView(
        computeSpriteFX.GetRootParameterIndex("ParticleInputBuffer"),
        scene.UpdateParticleResource->ParticleInput->GetGPUVirtualAddress());
    scene.CommandList->SetComputeRootShaderResourceView(
        computeSpriteFX.GetRootParameterIndex("EmitterInfoBuffer"),
        scene.UpdateParticleResource->EmitterInfo->GetGPUVirtualAddress());
    scene.CommandList->SetComputeRootUnorderedAccessView(
        computeSpriteFX.GetRootParameterIndex("ParticleOutputBuffer"),
        scene.RenderParticleResource->SimulationOutput->GetGPUVirtualAddress());

    UINT numThreadGroups = (scene.UpdateParticleResource->TotalCount + 31) / 32;
    scene.CommandList->Dispatch(numThreadGroups, 1, 1);

    computeOutputBarrier =
        CD3DX12_RESOURCE_BARRIER::Transition(scene.RenderParticleResource->SimulationOutput.Get(),
                                             D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
    scene.CommandList->ResourceBarrier(1, &computeOutputBarrier);
}

void ParticleManager::DispatchRibbon(float deltaTime, std::string sceneName)
{
    ParticleSceneResource& scene = _sceneResources.at(sceneName);
    if (scene.UpdateParticleResource->RibbonTotalCount == 0)
        return;

    CD3DX12_RESOURCE_BARRIER computeOutputBarrier =
        CD3DX12_RESOURCE_BARRIER::Transition(scene.RenderParticleResource->RibbonSimulationOutput.Get(),
                                             D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    scene.CommandList->ResourceBarrier(1, &computeOutputBarrier);

    scene.CommandList->SetComputeRootConstantBufferView(
        computeRibbonFX.GetRootParameterIndex("mvp"),
        scene.RenderParticleResource->MvpConstant->GetGPUVirtualAddress());
    scene.CommandList->SetComputeRootShaderResourceView(
        computeRibbonFX.GetRootParameterIndex("ParticleInputBuffer"),
        scene.UpdateParticleResource->RibbonParticleInput->GetGPUVirtualAddress());
    scene.CommandList->SetComputeRootShaderResourceView(
        computeRibbonFX.GetRootParameterIndex("EmitterInfoBuffer"),
        scene.UpdateParticleResource->RibbonEmitterInfo->GetGPUVirtualAddress());
    scene.CommandList->SetComputeRootUnorderedAccessView(
        computeRibbonFX.GetRootParameterIndex("ParticleOutputBuffer"),
        scene.RenderParticleResource->RibbonSimulationOutput->GetGPUVirtualAddress());

    UINT numThreadGroups = (UINT)(scene.UpdateParticleResource->RibbonTotalParticles.size() + 31) / 32;
    scene.CommandList->Dispatch(numThreadGroups, 1, 1);

    computeOutputBarrier =
        CD3DX12_RESOURCE_BARRIER::Transition(scene.RenderParticleResource->RibbonSimulationOutput.Get(),
                                             D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
    scene.CommandList->ResourceBarrier(1, &computeOutputBarrier);
}

void ParticleManager::UpdateMvpConstant(float deltaTime, ParticleRenderResource* sceneRenderResource)
{
    if (nullptr == sceneRenderResource)
        return;
    auto sceneCamera = Global::renderer->GetCamera(sceneRenderResource->Name);
    if (nullptr == sceneCamera)
        return;

    MVPConstants mvpConstants;
    mvpConstants.ViewMatrix = sceneCamera->GetViewMatrix().Transpose();
    Matrix viewrotinv       = sceneCamera->GetViewMatrix();

    XMFLOAT3X3 rotV;
    XMStoreFloat3x3(&rotV, viewrotinv);
    XMMATRIX Rv  = XMLoadFloat3x3(&rotV);
    XMMATRIX RvT = XMMatrixTranspose(Rv);
    XMStoreFloat4x4(&mvpConstants.ViewRotInvMatrix, RvT);

    mvpConstants.ViewRotInvMatrix = mvpConstants.ViewRotInvMatrix.Transpose();
    mvpConstants.ProjMatrix       = sceneCamera->GetProjectionMatrix().Transpose();
    mvpConstants.CameraPos        = Vector4(sceneCamera->GetWorldMatrix()._41, sceneCamera->GetWorldMatrix()._42,
                                            sceneCamera->GetWorldMatrix()._43, 1);
    mvpConstants.DeltaTime        = deltaTime;

    void* mappedData = nullptr;
    FAILED_CHECK_MESSAGE(sceneRenderResource->MvpConstant->Map(0, nullptr, &mappedData),
                         L"MVP Constant Buffer Map Failed");
    memcpy(mappedData, &mvpConstants, sizeof(MVPConstants));
    sceneRenderResource->MvpConstant->Unmap(0, nullptr);
}

void ParticleManager::UpdateLifeCycle(float deltaTime)
{
    std::set<std::shared_ptr<ParticleUpdateResource>> uniqueUpdateResources;
    for (const auto& pair : _sceneResources)
    {
        if (pair.second.UpdateParticleResource)
            uniqueUpdateResources.insert(pair.second.UpdateParticleResource);
    }

    for (const auto& updateResource : uniqueUpdateResources)
    {
        if (updateResource->Name == "ParticleEditor")
            continue;

        // Particle lifecycle
        for (const auto& effect : updateResource->SceneEffects)
        {
            if (effect->GetActiveFlag())
            {
                effect->UpdateParticleLifeCycle(deltaTime);
            }
        }

        // Effect lifecycle (removal)
        auto& effects = updateResource->SceneEffects;
        std::erase_if(effects, [](const auto& effect) { return effect->GetRemoveFlag(); });

        // Effect lifecycle (playing)
        for (const auto& effect : effects)
        {
            if (effect->GetPlayFlag())
            {
                effect->SetActiveFlag(true);
                effect->Reset();
                effect->SetPlayFlag(false);
            }
        }
    }
}

void ParticleManager::RefreshCurrentEditorEffect()
{
    if (auto sp = _editorCurrentEffect.lock())
    {
        sp->FlushEmitters();
        sp->Reset();
        sp->Play();
    }
    _editorRefreshFlag = false;
}