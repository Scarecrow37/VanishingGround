#include "pch.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"
#include "ParticleManager.h"

// =================================================================================================================
// [ 1. Constructor / Destructor & Initialize ]
// =================================================================================================================

ParticleManager::ParticleManager() = default;

ParticleManager::~ParticleManager() = default;

void ParticleManager::Initialize(UINT maxParticles)
{
    _maxParticles = maxParticles;
    InitializeComputeCommandObject();
    InitializeParticleComputeShader();
    InitializeParticleComputeRootSignature();
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

    std::wstring wSceneName(sceneName.begin(), sceneName.end());

    ParticleSceneResource newSceneResource;
    newSceneResource._name = sName;

    UINT particleInputSize  = _maxParticles * sizeof(Particle);
    UINT emitterInfoSize    = _maxEmitters * sizeof(EmitterInfo);
    UINT particleOutputSize = _maxParticles * sizeof(ParticleOutput);
    UINT mvpConstantSize    = sizeof(MVPConstants);

    newSceneResource._updateResource = std::make_shared<ParticleUpdateResource>();
    newSceneResource._updateResource->_name = sName;

    newSceneResource._renderResource = std::make_unique<ParticleRenderResource>();
    newSceneResource._renderResource->_name = sName;

    // Create GPU resources
    {
        // Input buffers (for UpdateResource)
        {
            CreateStructuredBuffer(newSceneResource._updateResource->_particleInput, newSceneResource._updateResource->_particleInputUpload, particleInputSize, sizeof(Particle));
            newSceneResource._updateResource->_particleInput->SetName((wSceneName + L" particle input").c_str());
            newSceneResource._updateResource->_particleInputUpload->SetName((wSceneName + L" particle input upload").c_str());

            CreateStructuredBuffer(newSceneResource._updateResource->_ribbonParticleInput, newSceneResource._updateResource->_ribbonParticleInputUpload, particleInputSize, sizeof(Particle));
            newSceneResource._updateResource->_ribbonParticleInput->SetName((wSceneName + L" ribbon particle input").c_str());
            newSceneResource._updateResource->_ribbonParticleInputUpload->SetName((wSceneName + L" ribbon particle input upload").c_str());
        }
        // Emitter info buffers (for UpdateResource)
        {
            CreateStructuredBuffer(newSceneResource._updateResource->_emitterInfo, newSceneResource._updateResource->_emitterInfoUpload, emitterInfoSize, sizeof(EmitterInfo));
            newSceneResource._updateResource->_emitterInfo->SetName((wSceneName + L" emitter info").c_str());
            newSceneResource._updateResource->_emitterInfoUpload->SetName((wSceneName + L" emitter info upload").c_str());

            CreateStructuredBuffer(newSceneResource._updateResource->_ribbonEmitterInfo, newSceneResource._updateResource->_ribbonEmitterInfoUpload, emitterInfoSize, sizeof(EmitterInfo));
            newSceneResource._updateResource->_ribbonEmitterInfo->SetName((wSceneName + L" ribbon emitter info").c_str());
            newSceneResource._updateResource->_ribbonEmitterInfoUpload->SetName((wSceneName + L" ribbon emitter info upload").c_str());
        }
        // Output buffers (for RenderResource)
        {
            CreateUAVBuffer(newSceneResource._renderResource->_simulationOutput, particleOutputSize, sizeof(ParticleOutput));
            newSceneResource._renderResource->_simulationOutput->SetName((wSceneName + L" output").c_str());

            CreateUAVBuffer(newSceneResource._renderResource->_ribbonSimulationOutput, particleOutputSize, sizeof(ParticleOutput));
            newSceneResource._renderResource->_ribbonSimulationOutput->SetName((wSceneName + L" ribbon output").c_str());
        }
        // MVP constant buffer (for RenderResource)
        {
            CreateConstantBuffer(newSceneResource._renderResource->_mvpConstant, mvpConstantSize);
            newSceneResource._renderResource->_mvpConstant->SetName((wSceneName + L" mvp constants").c_str());
        }
    }

    InitializeComputeCommandObject(newSceneResource);
    newSceneResource._updateResource->_totalParticles.resize(_maxParticles);
    newSceneResource._updateResource->_ribbonTotalParticles.resize(_maxParticles);

    _sceneResources.emplace(sName, std::move(newSceneResource));
}

void ParticleManager::AddSceneResource(std::string_view sceneName, std::string_view sharedFrom)
{
    auto sName = std::string(sceneName);
    auto sSharedFrom = std::string(sharedFrom);

    if (_sceneResources.count(sName) || !_sceneResources.count(sSharedFrom))
    {
        return; // Scene already exists or source scene doesn't exist
    }

    std::wstring wSceneName(sceneName.begin(), sceneName.end());

    ParticleSceneResource newSceneResource;
    newSceneResource._name = sName;

    UINT particleOutputSize = _maxParticles * sizeof(ParticleOutput);
    UINT mvpConstantSize    = sizeof(MVPConstants);

    // Share the update resource from the source scene
    newSceneResource._updateResource = _sceneResources.at(sSharedFrom)._updateResource;

    // Create a new unique render resource
    newSceneResource._renderResource = std::make_unique<ParticleRenderResource>();
    newSceneResource._renderResource->_name = sName;

    {
        CreateUAVBuffer(newSceneResource._renderResource->_simulationOutput, particleOutputSize, sizeof(ParticleOutput));
        newSceneResource._renderResource->_simulationOutput->SetName((wSceneName + L" output").c_str());

        CreateUAVBuffer(newSceneResource._renderResource->_ribbonSimulationOutput, particleOutputSize, sizeof(ParticleOutput));
        newSceneResource._renderResource->_ribbonSimulationOutput->SetName((wSceneName + L" ribbon output").c_str());

        CreateConstantBuffer(newSceneResource._renderResource->_mvpConstant, mvpConstantSize);
        newSceneResource._renderResource->_mvpConstant->SetName((wSceneName + L" mvp constants").c_str());
    }

    InitializeComputeCommandObject(newSceneResource);

    _sceneResources.emplace(sName, std::move(newSceneResource));
}

// =================================================================================================================
// [ 3. Effect Lifecycle Management ]
// =================================================================================================================

ParticleEffect* ParticleManager::RegisterEffect(EffectID id, const std::string& keyString, std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    if (!_sceneResources.count(sName)) return nullptr;

    auto newEffect = std::make_unique<ParticleEffect>();
    std::string name = "Effect" + std::to_string(nameingIndex++);
    newEffect->SetEffectName(name);

    ParticleEffect* rawPtr = newEffect.get();
    _sceneResources.at(sName)._updateResource->_sceneEffects.push_back(std::move(newEffect));

    _effectIDTable[id][keyString] = rawPtr;
    return rawPtr;
}

void ParticleManager::DeleteEffect(EffectID id, const std::string& keyString, const std::string& sceneName)
{
    auto effectIDIter = _effectIDTable.find(id);
    if (effectIDIter == _effectIDTable.end()) return;

    auto& effectMap = effectIDIter->second;
    auto targetEffectIter = effectMap.find(keyString);
    if (targetEffectIter == effectMap.end()) return;

    targetEffectIter->second->SetRemoveFlag(true);

    effectMap.erase(targetEffectIter);
    if (effectMap.empty())
    {
        _effectIDTable.erase(effectIDIter);
    }
}
class ParticleEffect* ParticleManager::FindEffect(EffectID id, const std::string& keyString)
{
    if (auto effectIDIter = _effectIDTable.find(id); effectIDIter != _effectIDTable.end())
    {
        auto& effectMap = effectIDIter->second;
        if (auto targetEffectIter = effectMap.find(keyString); targetEffectIter != effectMap.end())
        {
            return targetEffectIter->second;
        }
    }
    return nullptr;
}
void ParticleManager::PlayEffect(EffectID id, const std::string& keyString)
{
    if (auto effect = FindEffect(id, keyString)) effect->Play();
}

void ParticleManager::StopEffect(EffectID id, const std::string& keyString)
{
    if (auto effect = FindEffect(id, keyString)) effect->Stop();
}



// =================================================================================================================
// [ 4. Emitter Management ]
// =================================================================================================================

ParticleEmitter* ParticleManager::RegisterEmitter(ParticleEffect* effect, SIZE_T maxParticles,
                                                  float emissionRate, float emitterLifetime,
                                                  LocationShape     locatorShape,
                                                  Vector3           locationFactor,
                                                  ParticleType      particleType,
                                                  const std::wstring& meshspritePath)
{
    if (!effect) return nullptr;
    return effect->AddEmitter(maxParticles, emissionRate, emitterLifetime, locatorShape, locationFactor,
                              particleType, meshspritePath);
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
        if(pair.second._updateResource) uniqueUpdateResources.insert(pair.second._updateResource);
    }

    // 2. Update effects, awake particles, and copy to GPU
    _computeAllocator->Reset();
    _computeCommandList->Reset(_computeAllocator.Get(), nullptr);
    for (const auto& updateResource : uniqueUpdateResources)
    {
        for (const auto& effect : updateResource->_sceneEffects)
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
        if (scene._updateResource && !scene._updateResource->_sceneEffects.empty())
        {
            scene._commandAllocator->Reset();
            scene._commandList->Reset(scene._commandAllocator.Get(), nullptr);
            
            UpdateMvpConstant(deltaTime, scene._renderResource.get());

            scene._commandList->SetPipelineState(_computeSpritePSO.Get());
            scene._commandList->SetComputeRootSignature(_computeSpriteRootSignature.Get());
            DispatchSprite(deltaTime, scene._name);

            scene._commandList->SetPipelineState(_computeRibbonPSO.Get());
            scene._commandList->SetComputeRootSignature(_computeRibbonRootSignature.Get());
            DispatchRibbon(deltaTime, scene._name);

            scene._commandList->Close();
            Global::commandController->ExecuteCommand(COMPUTE_QUEUE, scene._commandList.Get());

            _computeFences[scene._name] = Global::commandController->SignalCommandQueue(COMPUTE_QUEUE);
        }
    }

    // 4. Update particle and effect lifecycles
    UpdateLifeCycle(delta);

    // 5. Handle editor-specific updates
    if (_editorCurrentEffect)
    {
        if (_editorCurrentEffect->GetActiveFlag())
        {
            _editorCurrentEffect->UpdateParticleLifeCycle(delta);
        }
        UpdateEditorLifeCycle();

        if (!_editorCurrentEffect->GetActiveFlag() && _isAutoRefresh)
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
    if (_sceneResources.count(sName))
    {
        return _sceneResources.at(sName)._updateResource->_totalCount;
    }
    return 0;
}

std::vector<Texture*> ParticleManager::GetActiveAlbedos(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    if (_sceneResources.count(sName))
    {
        return _sceneResources.at(sName)._updateResource->_activeEmitterAlbedos;
    }
    return {};
}

ID3D12Resource* ParticleManager::GetComputeOutputResource(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    if (_sceneResources.count(sName))
    {
        return _sceneResources.at(sName)._renderResource->_simulationOutput.Get();
    }
    return nullptr;
}

UINT ParticleManager::GetRibbonCount(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    if (_sceneResources.count(sName))
    {
        return _sceneResources.at(sName)._updateResource->_ribbonTotalCount;
    }
    return 0;
}

std::vector<std::vector<RibbonIndex>> ParticleManager::GetRibbonEmitterIndices(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    if (_sceneResources.count(sName))
    {
        return _sceneResources.at(sName)._updateResource->_ribbonIndices;
    }
    return {};
}

std::vector<Texture*> ParticleManager::GetActiveRibbonAlbedos(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    if (_sceneResources.count(sName))
    {
        return _sceneResources.at(sName)._updateResource->_ribbonActiveEmitterAlbedos;
    }
    return {};
}

ID3D12Resource* ParticleManager::GetRibbonOutputResource(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    if (_sceneResources.count(sName))
    {
        return _sceneResources.at(sName)._renderResource->_ribbonSimulationOutput.Get();
    }
    return nullptr;
}

UINT64 ParticleManager::GetComputeFenceValue(std::string_view sceneName)
{
    auto sName = std::string(sceneName);
    if (_computeFences.count(sName))
    {
        return _computeFences.at(sName);
    }
    return 0;
}

// =================================================================================================================
// [ 7. Setters ]
// =================================================================================================================

void ParticleManager::SetActiveFlag(EffectID id, const std::string& keyString, bool flag)
{
    if (auto effect = FindEffect(id, keyString)) effect->SetActiveFlag(flag);
}

void ParticleManager::SetRemoveFlag(EffectID id, const std::string& keyString, bool flag)
{
    if (auto effect = FindEffect(id, keyString)) effect->SetRemoveFlag(flag);
}

void ParticleManager::SetFollowBoneFlag(EffectID id, const std::string& keyString, bool* flag)
{
    if (auto effect = FindEffect(id, keyString)) effect->SetBoneFollowFlag(flag);
}

void ParticleManager::SetBoneMatrix(EffectID id, const std::string& keyString, const Matrix* boneMatrix)
{
    if (auto effect = FindEffect(id, keyString)) effect->SetBoneMatrix(boneMatrix);
}

// =================================================================================================================
// [ 8. Editor-specific ]
// =================================================================================================================

ParticleEffect* ParticleManager::RegisterEffectOnEditor()
{
    if (!_sceneResources.count("ParticleEditor"))
    {
        return nullptr;
    }

    auto& editorEffects = _sceneResources.at("ParticleEditor")._updateResource->_sceneEffects;

    // Clear any existing effects. unique_ptr will handle deletion.
    editorEffects.clear();

    auto newEffect = std::make_unique<ParticleEffect>();
    std::string name = "Effect" + std::to_string(nameingIndex++);
    newEffect->SetEffectName(name);

    _editorCurrentEffect = newEffect.get();
    editorEffects.push_back(std::move(newEffect));

    return _editorCurrentEffect;
}

void ParticleManager::SetCurrentEditorEffect(class ParticleEffect* newEffect)
{
    // This function now only sets the observing pointer. 
    // The actual object's lifetime is managed by the unique_ptr in the vector.
    _editorCurrentEffect = newEffect;
    RefreshEditor();
}

void ParticleManager::RefreshEditor()
{
    _editorRefreshFlag = true;
}

void ParticleManager::UpdateEditorLifeCycle()
{
    if (_editorCurrentEffect && _editorCurrentEffect->GetPlayFlag())
    {
        _editorCurrentEffect->SetPlayFlag(false);
        _editorCurrentEffect->Reset();
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
        if(pair.second._updateResource) uniqueUpdateResources.insert(pair.second._updateResource);
    }

    for (const auto& updateResource : uniqueUpdateResources)
    {
        for (const auto& effect : updateResource->_sceneEffects)
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
    hr = Global::device->GetDevice()->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(&scene._commandAllocator));
    FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeComputeCommandObject CreateCommandAllocator Failed");

    hr = Global::device->GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, scene._commandAllocator.Get(),
                                                       nullptr, IID_PPV_ARGS(scene._commandList.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeComputeCommandObject CreateCommandList Failed");
    scene._commandList->Close();
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
    hr = Global::device->GetDevice()->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(&_computeAllocator));
    FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeComputeCommandObject CreateCommandAllocator Failed");

    hr = Global::device->GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, _computeAllocator.Get(),
                                                       nullptr, IID_PPV_ARGS(_computeCommandList.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeComputeCommandObject CreateCommandList Failed");
    _computeCommandList->Close();
}

void ParticleManager::InitializeParticleComputeShader()
{
    HRESULT          hr = S_OK;
    ComPtr<ID3DBlob> error;

    // non-axial billboard sprite particle compute shader
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES |
                     D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
#ifdef _DEBUG
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        hr = D3DCompileFromFile(L"../Shaders/cs_compute_sprite.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                "cs_main", "cs_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
                                _computeSpriteShaderBlob.GetAddressOf(), error.GetAddressOf());

        if (error)
        {
            std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
            GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }
        FAILED_CHECK_MESSAGE(hr, L"D3DCompileFromFile cs_compute_sprite.hlsl Failed");
    }

    // ribbon sprite particle compute shader
    {
        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES |
                     D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
#ifdef _DEBUG
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        hr = D3DCompileFromFile(L"../Shaders/cs_compute_ribbon.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                "cs_main", "cs_5_1", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
                                _computeRibbonShaderBlob.GetAddressOf(), error.GetAddressOf());

        if (error)
        {
            std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
            GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }
        FAILED_CHECK_MESSAGE(hr, L"D3DCompileFromFile cs_compute_ribbon.hlsl Failed");
    }
}

void ParticleManager::InitializeParticleComputeRootSignature()
{
    // This logic is identical for both sprite and ribbon, could be refactored.
    std::vector<D3D12_ROOT_PARAMETER> rootParameters(4);

    rootParameters[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].Descriptor.ShaderRegister = 0; // b0
    rootParameters[0].Descriptor.RegisterSpace  = 0;
    rootParameters[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    rootParameters[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParameters[1].Descriptor.ShaderRegister = 0; // t0
    rootParameters[1].Descriptor.RegisterSpace  = 0;
    rootParameters[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    rootParameters[2].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParameters[2].Descriptor.ShaderRegister = 1; // t1
    rootParameters[2].Descriptor.RegisterSpace  = 0;
    rootParameters[2].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    rootParameters[3].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
    rootParameters[3].Descriptor.ShaderRegister = 0; // u0
    rootParameters[3].Descriptor.RegisterSpace  = 0;
    rootParameters[3].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_ROOT_SIGNATURE_DESC rootSignDesc;
    rootSignDesc.NumParameters     = (UINT)rootParameters.size();
    rootSignDesc.pParameters       = rootParameters.data();
    rootSignDesc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    rootSignDesc.NumStaticSamplers = 0;
    rootSignDesc.pStaticSamplers   = nullptr;

    ComPtr<ID3DBlob> serializedRootSig;
    ComPtr<ID3DBlob> error;
    HRESULT hr = D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                              serializedRootSig.GetAddressOf(), error.GetAddressOf());
    if (error)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
        GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
    }
    FAILED_CHECK_MESSAGE(hr, L"D3D12SerializeRootSignature Failed");

    hr = Global::device->GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
                                                          serializedRootSig->GetBufferSize(),
                                                          IID_PPV_ARGS(_computeSpriteRootSignature.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"CreateRootSignature for Sprite Failed");

    hr = Global::device->GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
                                                          serializedRootSig->GetBufferSize(),
                                                          IID_PPV_ARGS(_computeRibbonRootSignature.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"CreateRootSignature for Ribbon Failed");
}

void ParticleManager::InitializeParticleComputePSO()
{
    // initialize sprite pipeline state object
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC computePSODesc = {};
        computePSODesc.pRootSignature = _computeSpriteRootSignature.Get();
        computePSODesc.CS = {_computeSpriteShaderBlob->GetBufferPointer(), _computeSpriteShaderBlob->GetBufferSize()};
        HRESULT hr = Global::device->GetDevice()->CreateComputePipelineState(&computePSODesc, IID_PPV_ARGS(_computeSpritePSO.GetAddressOf()));
        FAILED_CHECK_MESSAGE(hr, L"CreateComputePipelineState for Sprite Failed");
    }
    // initialize ribbon pipeline state object
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC computePSODesc = {};
        computePSODesc.pRootSignature = _computeRibbonRootSignature.Get();
        computePSODesc.CS = {_computeRibbonShaderBlob->GetBufferPointer(), _computeRibbonShaderBlob->GetBufferSize()};
        HRESULT hr = Global::device->GetDevice()->CreateComputePipelineState(&computePSODesc, IID_PPV_ARGS(_computeRibbonPSO.GetAddressOf()));
        FAILED_CHECK_MESSAGE(hr, L"CreateComputePipelineState for Ribbon Failed");
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
    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    auto defaultProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    FAILED_CHECK_MESSAGE(Global::device->GetDevice()->CreateCommittedResource(&defaultProperty, D3D12_HEAP_FLAG_NONE,
                                                                              &bufferDesc, D3D12_RESOURCE_STATE_COMMON,
                                                                              nullptr, IID_PPV_ARGS(&resource)),
                         L"CreateCommittedResource for UAV buffer Failed");
}

void ParticleManager::CreateConstantBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize)
{
    UINT alignedBufferSize = (bufferSize + 255) & ~255;
    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(alignedBufferSize);
    auto uploadProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    FAILED_CHECK_MESSAGE(
        Global::device->GetDevice()->CreateCommittedResource(&uploadProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc,
                                                             D3D12_RESOURCE_STATE_GENERIC_READ,
                                                             nullptr, IID_PPV_ARGS(&resource)),
        L"CreateCommittedResource for constant buffer Failed");
}

void ParticleManager::AwakeParticles(float deltaTime, const std::shared_ptr<ParticleUpdateResource>& scene)
{
    scene->_emitterMatrix.clear();
    scene->_activeEmitterAlbedos.clear();
    scene->_ribbonEmitterMatrix.clear();
    scene->_ribbonActiveEmitterAlbedos.clear();
    scene->_ribbonIndices.clear();

    UINT totalSpriteParticles = 0;
    UINT totalRibbonParticles = 0;
    for (const auto& effect : scene->_sceneEffects)
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

    scene->_totalParticles.reserve(totalSpriteParticles);
    scene->_totalParticles.clear();
    scene->_ribbonTotalParticles.reserve(totalRibbonParticles);
    scene->_ribbonTotalParticles.clear();

    UINT emitterIndex = 0;
    UINT ribbonEmitterIndex = 0;
    UINT ribbonparticleIndex = 0;

    for (const auto& effect : scene->_sceneEffects)
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
                        scene->_activeEmitterAlbedos.push_back(spriteModule->GetAlbedoTexture());
                        Matrix worldMatrix = emitter->GetUseWorldSpace() ? Matrix::Identity : emitter->GetWorldMatrix().Transpose();
                        Matrix orientMatrix = emitter->GetWorldMatrix().Transpose();
                        scene->_emitterMatrix.push_back(
                            {worldMatrix, orientMatrix, emitter->GetDragPoint(), emitter->GetDragForce(), emitter->GetVortexForce(),
                             emitter->GetStartScale(), emitter->GetEndScale(),
                             Vector4(emitter->GetStartColor().x, emitter->GetStartColor().y, emitter->GetStartColor().z, emitter->GetStartOpacity()),
                             Vector4(emitter->GetEndColor().x, emitter->GetEndColor().y, emitter->GetEndColor().z, emitter->GetEndOpacity()),
                             Vector4(emitter->GetParticleLifetime(), emitter->GetUseWorldSpace() ? 1.0f : 0.0f, 0, 0),
                             Vector4(0, 0, 0, 0), Vector4(0, 0, 0, 0), Vector4(0, 0, 0, 0)
                            });

                        const auto& particlePool = emitter->GetParticlePool();
                        UINT activeCount = emitter->GetActiveParticleCount();

                        if (activeCount > 0)
                        {
                            size_t insert_position = scene->_totalParticles.size();
                            scene->_totalParticles.insert(scene->_totalParticles.end(), particlePool.begin(), particlePool.begin() + activeCount);
                            for (size_t i = 0; i < activeCount; ++i)
                            {
                                scene->_totalParticles[insert_position + i].SetEmitterIndex(emitterIndex);
                            }
                        }
                        emitterIndex++;
                    }
                    else if (ParticleType::RIBBON == emitter->_particleType)
                    {
                        auto ribbonModule = emitter->_particleRenderModule->AsRibbon();
                        scene->_ribbonActiveEmitterAlbedos.push_back(ribbonModule->GetAlbedoTexture());
                        Matrix worldMatrix =
                            emitter->GetUseWorldSpace() ? Matrix::Identity : emitter->GetWorldMatrix().Transpose();
                        
                        Matrix orientMatrix = emitter->GetWorldMatrix().Transpose();
                        scene->_ribbonEmitterMatrix.push_back(
                            {worldMatrix, orientMatrix, emitter->GetDragPoint(), emitter->GetDragForce(),
                             emitter->GetVortexForce(), emitter->GetStartScale(), emitter->GetEndScale(),
                             Vector4(emitter->GetStartColor().x, emitter->GetStartColor().y, emitter->GetStartColor().z,
                                     emitter->GetStartOpacity()),
                             Vector4(emitter->GetEndColor().x, emitter->GetEndColor().y, emitter->GetEndColor().z,
                                     emitter->GetEndOpacity()),
                             Vector4(emitter->GetParticleLifetime(), 0, 0, 0), ribbonModule->GetStartNormal(),
                             ribbonModule->GetEndNormal(), ribbonModule->GetRibbonVector()});

                        const auto& particlePool = emitter->GetParticlePool();
                        UINT activeCount = emitter->GetActiveParticleCount();

                        if (activeCount > 0)
                        {
                            std::vector<RibbonIndex> emitterIndices;
                            emitterIndices.reserve(activeCount * 2);
                            float lifetime = emitter->GetParticleLifetime();

                            for (UINT i = 0; i < activeCount; ++i)
                            {
                                Particle particle = particlePool[i];
                                particle.SetEmitterIndex(ribbonEmitterIndex);
                                scene->_ribbonTotalParticles.push_back(particle);
                                emitterIndices.push_back({ribbonparticleIndex++, particle.GetAge() / lifetime});
                                emitterIndices.push_back({ribbonparticleIndex++, particle.GetAge() / lifetime});
                            }

                            if (!emitterIndices.empty())
                            {
                                scene->_ribbonIndices.push_back(std::move(emitterIndices));
                            }
                        }
                        ribbonEmitterIndex++;
                    }
                }
            }
        }
    }
    scene->_totalCount = (UINT)scene->_totalParticles.size();
    scene->_ribbonTotalCount = (UINT)(scene->_ribbonTotalParticles.size() * 2);
}

void ParticleManager::UpdateAndCopyParticleResource(float deltaTime, const std::shared_ptr<ParticleUpdateResource>& scene)
{
    // memcpy to upload heap
    {
        void* mappedData = nullptr;
        if (scene->_totalCount > 0)
        {
            scene->_particleInputUpload->Map(0, nullptr, &mappedData);
            memcpy(mappedData, scene->_totalParticles.data(), scene->_totalCount * sizeof(Particle));
            scene->_particleInputUpload->Unmap(0, nullptr);

            scene->_emitterInfoUpload->Map(0, nullptr, &mappedData);
            memcpy(mappedData, scene->_emitterMatrix.data(), scene->_emitterMatrix.size() * sizeof(EmitterInfo));
            scene->_emitterInfoUpload->Unmap(0, nullptr);
        }
        if (scene->_ribbonTotalCount > 0)
        {
            scene->_ribbonParticleInputUpload->Map(0, nullptr, &mappedData);
            memcpy(mappedData, scene->_ribbonTotalParticles.data(), scene->_ribbonTotalParticles.size() * sizeof(Particle));
            scene->_ribbonParticleInputUpload->Unmap(0, nullptr);

            scene->_ribbonEmitterInfoUpload->Map(0, nullptr, &mappedData);
            memcpy(mappedData, scene->_ribbonEmitterMatrix.data(), scene->_ribbonEmitterMatrix.size() * sizeof(EmitterInfo));
            scene->_ribbonEmitterInfoUpload->Unmap(0, nullptr);
        }
    }

    // copy data from upload to default heap
    {
        CD3DX12_RESOURCE_BARRIER preCopyBarriers[] = {
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_particleInput.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_emitterInfo.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_ribbonParticleInput.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_ribbonEmitterInfo.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST)
        };
        _computeCommandList->ResourceBarrier(_countof(preCopyBarriers), preCopyBarriers);

        if (scene->_totalCount > 0)
        {
            UINT64 particleDataSize = scene->_totalParticles.size() * sizeof(Particle);
            _computeCommandList->CopyBufferRegion(scene->_particleInput.Get(), 0, scene->_particleInputUpload.Get(), 0, particleDataSize);

            UINT64 emitterDataSize = scene->_emitterMatrix.size() * sizeof(EmitterInfo);
            _computeCommandList->CopyBufferRegion(scene->_emitterInfo.Get(), 0, scene->_emitterInfoUpload.Get(), 0, emitterDataSize);
        }

        if (scene->_ribbonTotalCount > 0)
        {
            UINT64 particleDataSize = scene->_ribbonTotalParticles.size() * sizeof(Particle);
            _computeCommandList->CopyBufferRegion(scene->_ribbonParticleInput.Get(), 0, scene->_ribbonParticleInputUpload.Get(), 0, particleDataSize);

            UINT64 emitterDataSize = scene->_ribbonEmitterMatrix.size() * sizeof(EmitterInfo);
            _computeCommandList->CopyBufferRegion(scene->_ribbonEmitterInfo.Get(), 0, scene->_ribbonEmitterInfoUpload.Get(), 0, emitterDataSize);
        }

        CD3DX12_RESOURCE_BARRIER postCopyBarriers[] = {
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_particleInput.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_emitterInfo.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_ribbonParticleInput.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_ribbonEmitterInfo.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)
        };
        _computeCommandList->ResourceBarrier(_countof(postCopyBarriers), postCopyBarriers);
    }
}

void ParticleManager::DispatchSprite(float deltaTime, std::string sceneName)
{
    ParticleSceneResource& scene = _sceneResources.at(sceneName);
    if (scene._updateResource->_totalCount == 0) return;

    CD3DX12_RESOURCE_BARRIER computeOutputBarrier = CD3DX12_RESOURCE_BARRIER::Transition(scene._renderResource->_simulationOutput.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    scene._commandList->ResourceBarrier(1, &computeOutputBarrier);

    scene._commandList->SetComputeRootSignature(_computeSpriteRootSignature.Get());
    scene._commandList->SetComputeRootConstantBufferView(0, scene._renderResource->_mvpConstant->GetGPUVirtualAddress());
    scene._commandList->SetComputeRootShaderResourceView(1, scene._updateResource->_particleInput->GetGPUVirtualAddress());
    scene._commandList->SetComputeRootShaderResourceView(2, scene._updateResource->_emitterInfo->GetGPUVirtualAddress());
    scene._commandList->SetComputeRootUnorderedAccessView(3, scene._renderResource->_simulationOutput->GetGPUVirtualAddress());

    UINT numThreadGroups = (scene._updateResource->_totalCount + 31) / 32;
    scene._commandList->Dispatch(numThreadGroups, 1, 1);

    computeOutputBarrier = CD3DX12_RESOURCE_BARRIER::Transition(scene._renderResource->_simulationOutput.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
    scene._commandList->ResourceBarrier(1, &computeOutputBarrier);
}

void ParticleManager::DispatchRibbon(float deltaTime, std::string sceneName)
{
    ParticleSceneResource& scene = _sceneResources.at(sceneName);
    if (scene._updateResource->_ribbonTotalCount == 0) return;

    CD3DX12_RESOURCE_BARRIER computeOutputBarrier = CD3DX12_RESOURCE_BARRIER::Transition(scene._renderResource->_ribbonSimulationOutput.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    scene._commandList->ResourceBarrier(1, &computeOutputBarrier);

    scene._commandList->SetComputeRootSignature(_computeRibbonRootSignature.Get());
    scene._commandList->SetComputeRootConstantBufferView(0, scene._renderResource->_mvpConstant->GetGPUVirtualAddress());
    scene._commandList->SetComputeRootShaderResourceView(1, scene._updateResource->_ribbonParticleInput->GetGPUVirtualAddress());
    scene._commandList->SetComputeRootShaderResourceView(2, scene._updateResource->_ribbonEmitterInfo->GetGPUVirtualAddress());
    scene._commandList->SetComputeRootUnorderedAccessView(3, scene._renderResource->_ribbonSimulationOutput->GetGPUVirtualAddress());

    UINT numThreadGroups = (UINT)(scene._updateResource->_ribbonTotalParticles.size() + 31) / 32;
    scene._commandList->Dispatch(numThreadGroups, 1, 1);

    computeOutputBarrier = CD3DX12_RESOURCE_BARRIER::Transition(scene._renderResource->_ribbonSimulationOutput.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
    scene._commandList->ResourceBarrier(1, &computeOutputBarrier);
}

void ParticleManager::UpdateMvpConstant(float deltaTime, ParticleRenderResource* sceneRenderResource)
{
    if (nullptr == sceneRenderResource)
        return;
    auto sceneCamera = Global::renderer->GetCamera(sceneRenderResource->_name);
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
    mvpConstants.deltaTime = deltaTime;

    void* mappedData = nullptr;
    FAILED_CHECK_MESSAGE(sceneRenderResource->_mvpConstant->Map(0, nullptr, &mappedData), L"MVP Constant Buffer Map Failed");
    memcpy(mappedData, &mvpConstants, sizeof(MVPConstants));
    sceneRenderResource->_mvpConstant->Unmap(0, nullptr);
}

void ParticleManager::UpdateLifeCycle(float deltaTime)
{
    std::set<std::shared_ptr<ParticleUpdateResource>> uniqueUpdateResources;
    for (const auto& pair : _sceneResources)
    {
        if (pair.second._updateResource) uniqueUpdateResources.insert(pair.second._updateResource);
    }

    for (const auto& updateResource : uniqueUpdateResources)
    {
        if (updateResource->_name == "ParticleEditor") continue;

        // Particle lifecycle
        for (const auto& effect : updateResource->_sceneEffects)
        {
            if (effect->GetActiveFlag())
            {
                effect->UpdateParticleLifeCycle(deltaTime);
            }
        }

        // Effect lifecycle (removal)
        auto& effects = updateResource->_sceneEffects;
        std::erase_if(effects, [](const auto& effect) {
            return effect->GetRemoveFlag();
        });

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
    if (_editorCurrentEffect)
    {
        _editorCurrentEffect->FlushEmitters();
        _editorCurrentEffect->Reset();
        _editorCurrentEffect->Play();
    }
    _editorRefreshFlag = false;
}