#include "pch.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"

ParticleManager::ParticleManager() {}

ParticleManager::~ParticleManager()
{
    // _editorCurrentEffect는 _sceneResources가 관리하는 이펙트를 가리키는 관찰 포인터이므로,
    // 여기서 직접 delete하지 않고 nullptr로 초기화하여 댕글링 포인터 접근을 방지합니다.
    // 실제 메모리는 아래의 로직에서 해제됩니다.
    _editorCurrentEffect = nullptr;

    // 1. 모든 고유한 ParticleUpdateResource와 그들이 소유한 ParticleEffect 객체들을 정리합니다.
    //    _particleUpdateResources 셋을 순회하면 공유된 리소스가 단 한 번만 삭제되는 것을 보장할 수 있습니다.
    for (ParticleUpdateResource* updateResource : _particleUpdateResources)
    {
        if (updateResource)
        {
            // 이 업데이트 리소스가 소유한 모든 ParticleEffect 객체를 삭제합니다.
            for (ParticleEffect* effect : updateResource->_sceneEffects)
            {
                if (effect)
                {
                    delete effect;
                }
            }
            updateResource->_sceneEffects.clear(); // 포인터 벡터를 비웁니다.

            // 이제 업데이트 리소스 컨테이너 자체를 삭제합니다.
            delete updateResource;
        }
    }
    _particleUpdateResources.clear(); // 포인터 셋을 비웁니다.

    // 2. 모든 ParticleRenderResource 객체를 정리합니다.
    //    이 리소스들은 각 ParticleSceneResource가 고유하게 소유합니다.
    for (auto& pair : _sceneResources)
    {
        ParticleSceneResource& sceneResource = pair.second;
        if (sceneResource._renderResource)
        {
            delete sceneResource._renderResource;
        }
    }
    _sceneResources.clear(); // 맵과 그 안의 (포인터가 아닌) 객체들이 소멸됩니다.
}

void ParticleManager::Initialize(UINT maxParticles)
{
    _currentBufferIndex = 0;
    _particleStride     = sizeof(Particle);
    _maxParticles       = maxParticles;
    _totalParticles.resize(_maxParticles);

    InitializeComputeCommandObject();
    InitializeParticleComputeShader();
    InitializeParticleComputeRootSignature();
    InitializeParticleComputePSO();

}

ParticleEffect* ParticleManager::RegisterEffect(std::string_view sceneName)
{
    auto newEffect = new ParticleEffect();
    newEffect->Initialize(this);
    std::string name = "Effect" + std::to_string(nameingIndex++);
    newEffect->SetEffectName(name);

    auto scenename = std::string(sceneName);
    _sceneResources[scenename]._updateResource->_sceneEffects.push_back(newEffect);
    return newEffect;
}

class ParticleEffect* ParticleManager::RegisterEffectOnEditor()
{
    auto newEffect = new ParticleEffect();
    newEffect->Initialize(this);
    std::string name = "Effect" + std::to_string(nameingIndex++);
    newEffect->SetEffectName(name);

    _sceneResources["ParticleEditor"]._updateResource->_sceneEffects.resize(1);
    if (nullptr != _sceneResources["ParticleEditor"]._updateResource->_sceneEffects[0])
        delete _sceneResources["ParticleEditor"]._updateResource->_sceneEffects[0];

    _sceneResources["ParticleEditor"]._updateResource->_sceneEffects[0] = newEffect;

    _editorCurrentEffect = newEffect;
    return newEffect;
}

void ParticleManager::ChangeTexture()
{
    for (auto& updateresource : _particleUpdateResources)
    {
        for (auto& effect : updateresource->_sceneEffects)
        {
            if (true == effect->GetActiveFlag())
            {
                for (auto& emitter : effect->GetEmitterList())
                {
                    emitter->FlushTextureResource();
                }
            }
        }
    }
    if (true == _editorRefreshFlag)
        RefreshCurrentEditorEffect();

}

ParticleEmitter* ParticleManager::RegisterEmitter(class ParticleEffect* effect, SIZE_T maxParticles /*= 100000*/,
                                                  float emissionRate /*= 500.f*/, float emitterLifetime /*= 5.f*/,
                                                  LocationShape     locatorShape /*= LocationShape::SPHERE*/,
                                                  Vector3           locationFactor /*= Vector3(1, 1, 1)*/,
                                                  ParticleType      particleType /*= ParticleType::SPRITE*/,
                                                  std::wstring_view meshspritePath /*= L""*/)
{
    auto newEmitter = effect->AddEmitter(maxParticles, emissionRate, emitterLifetime, locatorShape, locationFactor,
                                         particleType, meshspritePath);
    return newEmitter;
}

void ParticleManager::DeleteEffect(ParticleEffect* target)
{
    target->SetRemoveFlag(true);

    // erase_if 전에 메모리 해제
    for (auto it = _particleEffects.begin(); it != _particleEffects.end();)
    {
        if (target == (*it))
        {
            delete *it; // 메모리 해제
            it = _particleEffects.erase(it);
        }
        else
        {
            ++it;
        }
    }
}
void ParticleManager::Update(const float deltaTime)
{
    float delta = deltaTime * _deltaScale;

    {
        _computeAllocator->Reset();
        _computeCommandList->Reset(_computeAllocator.Get(), nullptr);
        for (auto& updateresource : _particleUpdateResources)
        {
            for (auto& effect : updateresource->_sceneEffects)
            {
                if (true == effect->GetActiveFlag())
                {
                    effect->Update(delta);
                }
            }
            AwakeParticles(deltaTime, updateresource);
            UpdateAndCopyParticleResource(deltaTime, updateresource);
        }
        _computeCommandList->Close();
        Global::commandController->ExecuteCommand(COMPUTE_QUEUE, _computeCommandList.Get());
    }

    {
 
        for (auto& scene : _sceneResources)
        {
            if (false == scene.second._updateResource->_sceneEffects.empty())
            {
                scene.second._commandAllocator->Reset();
                scene.second._commandList->Reset(scene.second._commandAllocator.Get(), nullptr);
                UpdateMvpConstant(deltaTime, scene.second._renderResource);

                scene.second._commandList->SetPipelineState(_computeSpritePSO.Get());
                scene.second._commandList->SetComputeRootSignature(_computeSpriteRootSignature.Get());
                DispatchSprite(deltaTime, scene.second._name);
                scene.second._commandList->SetPipelineState(_computeRibbonPSO.Get());
                scene.second._commandList->SetComputeRootSignature(_computeRibbonRootSignature.Get());
                DispatchRibbon(deltaTime, scene.second._name);

                scene.second._commandList->Close();
                Global::commandController->ExecuteCommand(COMPUTE_QUEUE, scene.second._commandList.Get());

                _computeFences[scene.second._name] = Global::commandController->SignalCommandQueue(COMPUTE_QUEUE);

            }
        }
    }
    UpdateLifeCycle(deltaTime);


    if (nullptr != _editorCurrentEffect)
    {
        if (true == _editorCurrentEffect->GetActiveFlag())
        {
            _editorCurrentEffect->UpdateParticleLifeCycle(delta);
        }
        UpdateEditorLifeCycle();

        if (false == _editorCurrentEffect->GetActiveFlag() && true == _isAutoRefresh)
            RefreshEditor();


    }
}

void ParticleManager::UpdateLifeCycle(float deltaTime) 
{
    //particle lifecycle
    for (auto& updateresource : _particleUpdateResources)
    {
        if ("ParticleEditor" == updateresource->_name)
            continue;
        for (auto& effect : updateresource->_sceneEffects)
        {
            if (true == effect->GetActiveFlag())
            {
                effect->UpdateParticleLifeCycle(deltaTime);
            }
        }
    }

    // effect lifecycle
    for (auto& updateresource : _particleUpdateResources)
    {
        if ("ParticleEditor" == updateresource->_name)
            continue;
        // erase_if 전에 메모리 해제
        for (auto it = updateresource->_sceneEffects.begin(); it != updateresource->_sceneEffects.end();)
        {
            if (true == (*it)->GetRemoveFlag())
            {
                delete *it; // 메모리 해제
                it = updateresource->_sceneEffects.erase(it);
            }
            else
            {
                ++it;
            }
        }

        for (auto newEffect : updateresource->_sceneEffects)
        {
            if (true == newEffect->GetPlayFlag())
            {
                newEffect->SetActiveFlag(true);
                newEffect->Reset();
                newEffect->SetPlayFlag(false);
            }
        }
    }
}
void ParticleManager::UpdateEditorLifeCycle()
{
    if (true == _editorCurrentEffect->GetPlayFlag())
    {
        _editorCurrentEffect->SetPlayFlag(false);
        _editorCurrentEffect->Reset();
    }
}
void ParticleManager::RefreshEditor()
{
    _editorRefreshFlag = true;
}


UINT ParticleManager::GetMaxCount() 
{
    return _maxParticles;
}

UINT ParticleManager::GetTotalCount(std::string_view sceneName) 
{

    auto scenename = std::string(sceneName);
    return _sceneResources[scenename]._updateResource->_totalCount;

}

std::vector<Texture*> ParticleManager::GetActiveAlbedos(std::string_view sceneName) 
{
    auto scenename = std::string(sceneName);
    return _sceneResources[scenename]._updateResource->_activeEmitterAlbedos;
}

ID3D12Resource* ParticleManager::GetComputeOutputResource(std::string_view sceneName)
{
    auto scenename = std::string(sceneName);
    return _sceneResources[scenename]._renderResource->_simulationOutput.Get();
}

UINT ParticleManager::GetRibbonCount(std::string_view sceneName) 
{
    auto scenename = std::string(sceneName);
    return _sceneResources[scenename]._updateResource->_ribbonTotalCount;
}

std::vector<std::vector<ribbonIndex>> ParticleManager::GetRibbonEmitterIndices(std::string_view sceneName) 
{
    auto scenename = std::string(sceneName);
    return _sceneResources[scenename]._updateResource->_ribbonIndices;
}

std::vector<Texture*> ParticleManager::GetActiveRibbonAlbedos(std::string_view sceneName) 
{
    auto scenename = std::string(sceneName);
    return _sceneResources[scenename]._updateResource->_ribbonActiveEmitterAlbedos;
}

ID3D12Resource* ParticleManager::GetRibbonOutputResource(std::string_view sceneName)
{
    auto scenename = std::string(sceneName);
    return _sceneResources[scenename]._renderResource->_ribbonSimulationOutput.Get();
}

UINT64 ParticleManager::GetComputeFenceValue(std::string_view sceneName)
{
    auto scenename = std::string(sceneName);
    return _computeFences[scenename];
}

void ParticleManager::SetCamera(std::string_view viewName)
{
    _camera = Global::renderer->GetCamera(viewName);
}
void ParticleManager::SetCamera(std::shared_ptr<Camera> camera)
{
    _camera = camera;
}
void ParticleManager::SetCurrentEditorEffect(class ParticleEffect* newEffect)
{
    if (_editorCurrentEffect == nullptr)
        delete _editorCurrentEffect;
    _editorCurrentEffect = newEffect;
    RefreshEditor();
}
void ParticleManager::InitializeComputeCommandObject(std::string_view sceneName)
{
    auto scenename = std::string(sceneName);
    ParticleSceneResource& scene     = _sceneResources[scenename];

    D3D12_COMMAND_QUEUE_DESC desc{
        .Type     = D3D12_COMMAND_LIST_TYPE_COMPUTE,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
    };
    {
        HRESULT hr = S_OK;
        hr         = Global::device->GetDevice()->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(&scene._commandAllocator));
        FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeComputeCommandObject "
                                 L"Global::device->GetDevice()->CreateCommandAllocator Failed");

        FAILED_CHECK_MESSAGE(
            Global::device->GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, scene._commandAllocator.Get(),
                                                           nullptr,
                                 IID_PPV_ARGS(scene._commandList.GetAddressOf())),
            L"");
        scene._commandList->Close();
    }
}

void ParticleManager::InitializeComputeCommandObject() 
{

    D3D12_COMMAND_QUEUE_DESC desc{
        .Type     = D3D12_COMMAND_LIST_TYPE_COMPUTE,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
    };
    {
        HRESULT hr = S_OK;
        hr = Global::device->GetDevice()->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(&_computeAllocator));
        FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeComputeCommandObject "
                                 L"Global::device->GetDevice()->CreateCommandAllocator Failed");

        FAILED_CHECK_MESSAGE(
            Global::device->GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, _computeAllocator.Get(),
                                                           nullptr, IID_PPV_ARGS(_computeCommandList.GetAddressOf())),
            L"");
        _computeCommandList->Close();
    }
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

        hr = D3DCompileFromFile(L"../Shaders/cs_compute_sprite.hlsl", // HLSL 파일 경로
                                nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                "cs_main", // 셰이더 진입점
                                "cs_5_1",  // 셰이더 모델
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
                                _computeSpriteShaderBlob.GetAddressOf(), error.GetAddressOf());

        if (nullptr != error)
        {

            std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
            GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }

        FAILED_CHECK_MESSAGE(hr, L"D3DCompileFromFile Failed");
    }
    // ribbon sprite particle compute shader
    {

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES |
                     D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

#ifdef _DEBUG
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        hr = D3DCompileFromFile(L"../Shaders/cs_compute_ribbon.hlsl", // HLSL 파일 경로
                                nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                "cs_main", // 셰이더 진입점
                                "cs_5_1",  // 셰이더 모델
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
                                _computeRibbonShaderBlob.GetAddressOf(), error.GetAddressOf());

        if (nullptr != error)
        {
            std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
            GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }

        FAILED_CHECK_MESSAGE(hr, L"D3DCompileFromFile Failed");
    }
}
void ParticleManager::InitializeParticleComputeRootSignature()
{
    // initialize sprite root signature;
    {

        std::vector<D3D12_ROOT_PARAMETER> rootParameters;
        rootParameters.resize(4);

        rootParameters[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[0].Descriptor.ShaderRegister = 0; // b0
        rootParameters[0].Descriptor.RegisterSpace  = 0;
        rootParameters[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

        // SRV (t0)
        rootParameters[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParameters[1].Descriptor.ShaderRegister = 0; // t0
        rootParameters[1].Descriptor.RegisterSpace  = 0;
        rootParameters[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

        // SRV (t1)
        rootParameters[2].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParameters[2].Descriptor.ShaderRegister = 1; // t1
        rootParameters[2].Descriptor.RegisterSpace  = 0;
        rootParameters[2].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

        // UAV (u0)
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
        HRESULT          hr = D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                                          serializedRootSig.GetAddressOf(), error.GetAddressOf());
        if (nullptr != error)
        {
            std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
            GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }

        FAILED_CHECK_MESSAGE(
            hr, L"ParticleManager::InitializeParticleComputeRootSignature D3D12SerializeRootSignature Failed");

        ComPtr<ID3D12RootSignature> rootSignature;
        hr = Global::device->GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
                                                              serializedRootSig->GetBufferSize(),
                                                              IID_PPV_ARGS(_computeSpriteRootSignature.GetAddressOf()));
        FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeParticleComputeRootSignature CreateRootSignature Failed");
    }
    // initialize ribbon root signature;
    {

        std::vector<D3D12_ROOT_PARAMETER> rootParameters;
        rootParameters.resize(4);

        rootParameters[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParameters[0].Descriptor.ShaderRegister = 0; // b0
        rootParameters[0].Descriptor.RegisterSpace  = 0;
        rootParameters[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

        // SRV (t0)
        rootParameters[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParameters[1].Descriptor.ShaderRegister = 0; // t0
        rootParameters[1].Descriptor.RegisterSpace  = 0;
        rootParameters[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

        // SRV (t1)
        rootParameters[2].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
        rootParameters[2].Descriptor.ShaderRegister = 1; // t1
        rootParameters[2].Descriptor.RegisterSpace  = 0;
        rootParameters[2].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

        // UAV (u0)
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
        HRESULT          hr = D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                                          serializedRootSig.GetAddressOf(), error.GetAddressOf());
        if (nullptr != error)
        {
            std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
            GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }

        FAILED_CHECK_MESSAGE(hr, L"");

        ComPtr<ID3D12RootSignature> rootSignature;
        hr = Global::device->GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
                                                              serializedRootSig->GetBufferSize(),
                                                              IID_PPV_ARGS(_computeRibbonRootSignature.GetAddressOf()));
        FAILED_CHECK_MESSAGE(hr, L"");
    }
}
void ParticleManager::InitializeParticleComputePSO()
{
    // initialize sprite pipeline state object
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC computePSODesc = {};
        ZeroMemory(&computePSODesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
        computePSODesc.CS = {_computeSpriteShaderBlob->GetBufferPointer(), _computeSpriteShaderBlob->GetBufferSize()};
        computePSODesc.pRootSignature = _computeSpriteRootSignature.Get();
        HRESULT hr;
        hr = Global::device->GetDevice()->CreateComputePipelineState(&computePSODesc,
                                                                     IID_PPV_ARGS(_computeSpritePSO.GetAddressOf()));
        FAILED_CHECK_MESSAGE(hr, L"");
    }
    // initialize ribbon pipeline state object
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC computePSODesc = {};
        ZeroMemory(&computePSODesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
        computePSODesc.CS = {_computeRibbonShaderBlob->GetBufferPointer(), _computeRibbonShaderBlob->GetBufferSize()};
        computePSODesc.pRootSignature = _computeRibbonRootSignature.Get();

        HRESULT hr;
        hr = Global::device->GetDevice()->CreateComputePipelineState(&computePSODesc,
                                                                     IID_PPV_ARGS(_computeRibbonPSO.GetAddressOf()));
        FAILED_CHECK_MESSAGE(hr, L"");
    }
}
void ParticleManager::InitializeDescriptorHeap()
{
    // 디스크립터 힙 생성 (CBV 1개 + SRV 2개 + UAV 1개 = 총 4개)
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = 4;
    heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    FAILED_CHECK_MESSAGE(
        Global::device->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_cbvSrvUavHeap.GetAddressOf())), L"");

    _descriptorSize =
        Global::device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void ParticleManager::CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, ComPtr<ID3D12Resource>& uploadResource,
                                             UINT bufferSize, UINT stride)
{
    // 기본 버퍼 생성 (GPU 전용)
    D3D12_RESOURCE_DESC bufferDesc      = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    auto                defaultProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    FAILED_CHECK_MESSAGE(Global::device->GetDevice()->CreateCommittedResource(&defaultProperty, D3D12_HEAP_FLAG_NONE,
                                                                              &bufferDesc, D3D12_RESOURCE_STATE_COMMON,
                                                                              nullptr, IID_PPV_ARGS(&resource)),
                         L"");

    // 업로드 버퍼 생성 (CPU->GPU 전송용)
    auto uploadProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    FAILED_CHECK_MESSAGE(Global::device->GetDevice()->CreateCommittedResource(
                             &uploadProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                             nullptr, IID_PPV_ARGS(&uploadResource)),
                         L"");
}
void ParticleManager::CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride)
{
    // uav로 쓰고 srv로 읽기용
    D3D12_RESOURCE_DESC bufferDesc      = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    auto                defaultProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    FAILED_CHECK_MESSAGE(Global::device->GetDevice()->CreateCommittedResource(&defaultProperty, D3D12_HEAP_FLAG_NONE,
                                                                              &bufferDesc, D3D12_RESOURCE_STATE_COMMON,
                                                                              nullptr, IID_PPV_ARGS(&resource)),
                         L"");
}
void ParticleManager::CreateUAVBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride)
{
    // UAV 버퍼 생성 (GPU 읽기/쓰기)
    D3D12_RESOURCE_DESC bufferDesc =
        CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    auto defaultProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    FAILED_CHECK_MESSAGE(Global::device->GetDevice()->CreateCommittedResource(&defaultProperty, D3D12_HEAP_FLAG_NONE,
                                                                              &bufferDesc, D3D12_RESOURCE_STATE_COMMON,
                                                                              nullptr, IID_PPV_ARGS(&resource)),
                         L"");
}
void ParticleManager::CreateConstantBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize)
{
    // 상수 버퍼는 항상 256바이트 정렬되어야 함
    UINT alignedBufferSize = (bufferSize + 255) & ~255;

    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(alignedBufferSize);

    // 2. Upload Heap에 업로드 버퍼 생성 (CPU 접근 가능)
    auto uploadProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    FAILED_CHECK_MESSAGE(
        Global::device->GetDevice()->CreateCommittedResource(&uploadProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc,
                                                             D3D12_RESOURCE_STATE_GENERIC_READ, // Upload Heap 필수 상태
                                                             nullptr, IID_PPV_ARGS(&resource)),
        L"");
}

void ParticleManager::AwakeParticles(float deltaTime, ParticleUpdateResource* scene)
{
    // 메타데이터 벡터 초기화
    scene->_emitterMatrix.clear();
    scene->_activeEmitterAlbedos.clear();
    scene->_ribbonEmitterMatrix.clear();
    scene->_ribbonActiveEmitterAlbedos.clear();
    scene->_ribbonIndices.clear();

    // 1. 이번 프레임에 필요한 총 파티클 수 미리 계산
    UINT totalSpriteParticles = 0;
    UINT totalRibbonParticles = 0;
    for (auto effect : scene->_sceneEffects)
    {
        if (effect->GetActiveFlag())
        {
            for (auto emitter : effect->GetEmitterList())
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

    // 2. 필요한 메모리 공간을 한 번에 예약
    scene->_totalParticles.reserve(totalSpriteParticles);
    scene->_totalParticles.clear(); 
    scene->_ribbonTotalParticles.reserve(totalRibbonParticles);
    scene->_ribbonTotalParticles.clear();

    UINT emitterIndex = 0;
    UINT ribbonEmitterIndex = 0;
    UINT ribbonparticleIndex = 0;

    // 3. 파티클 데이터 복사
    for (auto effect : scene->_sceneEffects)
    {
        if (true == effect->GetActiveFlag())
        {
            for (auto emitter : effect->GetEmitterList())
            {
                if (true == emitter->GetActiveFlag())
                {
                    if (ParticleType::SPRITE == emitter->_particleType)
                    {
                        // (EmitterInfo 설정 코드는 기존과 동일)
                        scene->_activeEmitterAlbedos.push_back(
                            static_cast<SpriteModule*>(emitter->_particleRenderModule)->GetAlbedoTexture());
                        Matrix worldMatrix =
                            emitter->GetUseWorldSpace() ? Matrix::Identity : emitter->GetWorldMatrix().Transpose();
                        Matrix orientMatrix = emitter->GetWorldMatrix().Transpose();
                        scene->_emitterMatrix.push_back(
                            {worldMatrix,orientMatrix, emitter->GetDragPoint(), emitter->GetDragForce(), emitter->GetVortexForce(),
                             emitter->GetStartScale(), emitter->GetEndScale(),
                             Vector4(emitter->GetStartColor().x, emitter->GetStartColor().y, emitter->GetStartColor().z,
                                     emitter->GetStartOpacity()),
                             Vector4(emitter->GetEndColor().x, emitter->GetEndColor().y, emitter->GetEndColor().z,
                                     emitter->GetEndOpacity()),
                             Vector4(emitter->GetParticleLifetime(), emitter->GetUseWorldSpace() ? 1.0f : 0.0f, 0, 0),
                             Vector4(0, 0, 0, 0), Vector4(0, 0, 0, 0),  Vector4(0, 0, 0, 0)
                            });

                        auto& particlePool = emitter->GetParticlePool();
                        UINT activeCount = emitter->GetActiveParticleCount();

                        if (activeCount > 0)
                        {
                            size_t insert_position = scene->_totalParticles.size();
                            // 활성 파티클 블록 전체를 한 번에 삽입
                            scene->_totalParticles.insert(scene->_totalParticles.end(), particlePool.begin(), particlePool.begin() + activeCount);

                            // 복사된 파티클들에 EmitterIndex 설정
                            for (size_t i = 0; i < activeCount; ++i)
                            {
                                scene->_totalParticles[insert_position + i].SetEmitterIndex(emitterIndex);
                            }
                        }
                        emitterIndex++;
                    }
                    else if (ParticleType::RIBBON == emitter->_particleType)
                    {
                        // (Ribbon EmitterInfo 설정 코드는 기존과 동일)
                        scene->_ribbonActiveEmitterAlbedos.push_back(
                            static_cast<RibbonModule*>(emitter->_particleRenderModule)->GetAlbedoTexture());
                        Matrix worldMatrix =
                            emitter->GetUseWorldSpace() ? Matrix::Identity : emitter->GetWorldMatrix().Transpose();
                        Matrix orientMatrix = emitter->GetWorldMatrix().Transpose();
                        scene->_ribbonEmitterMatrix.push_back(
                            {worldMatrix, orientMatrix,emitter->GetDragPoint(), emitter->GetDragForce(),
                             emitter->GetVortexForce(),
                             emitter->GetStartScale(), emitter->GetEndScale(),
                             Vector4(emitter->GetStartColor().x, emitter->GetStartColor().y, emitter->GetStartColor().z,
                                     emitter->GetStartOpacity()),
                             Vector4(emitter->GetEndColor().x, emitter->GetEndColor().y, emitter->GetEndColor().z,
                                     emitter->GetEndOpacity()),
                             Vector4(emitter->GetParticleLifetime(), 0, 0, 0),
                             static_cast<RibbonModule*>(emitter->_particleRenderModule)->GetStartNormal(),
                             static_cast<RibbonModule*>(emitter->_particleRenderModule)->GetEndNormal(),
                            static_cast<RibbonModule*>(emitter->_particleRenderModule)->GetRibbonVector()
                            }

                        );

                        auto& particlePool = emitter->GetParticlePool();
                        UINT activeCount = emitter->GetActiveParticleCount();
                        
                        if (activeCount > 0)
                        {
                            std::vector<ribbonIndex> emitterIndices;
                            emitterIndices.reserve(activeCount * 2);
                            float lifetime = emitter->GetParticleLifetime();

                            for (UINT i = 0; i < activeCount; ++i)
                            {
                                Particle particle = particlePool[i];
                                particle.SetEmitterIndex(ribbonEmitterIndex);

                                scene->_ribbonTotalParticles.push_back(particle);
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
    // 최종 파티클 수 업데이트
    scene->_totalCount = scene->_totalParticles.size();
    scene->_ribbonTotalCount = scene->_ribbonTotalParticles.size();
}

void ParticleManager::DispatchSprite(float deltaTime, std::string sceneName) 
{
    ParticleSceneResource& scene = _sceneResources[sceneName];
    {
        if (0 >= scene._updateResource->_totalCount)
            return;
        CD3DX12_RESOURCE_BARRIER computeOutputBarrior =
            CD3DX12_RESOURCE_BARRIER::Transition(scene._renderResource->_simulationOutput.Get(),
                                                 D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        scene._commandList->ResourceBarrier(1, &computeOutputBarrior);

        scene._commandList->SetComputeRootSignature(_computeSpriteRootSignature.Get());

        scene._commandList->SetComputeRootConstantBufferView(
            0, scene._renderResource->_mvpConstant->GetGPUVirtualAddress());
        scene._commandList->SetComputeRootShaderResourceView(
            1, scene._updateResource->_particleInput->GetGPUVirtualAddress());
        scene._commandList->SetComputeRootShaderResourceView(
            2, scene._updateResource->_emitterInfo->GetGPUVirtualAddress());
        scene._commandList->SetComputeRootUnorderedAccessView(
            3, scene._renderResource->_simulationOutput->GetGPUVirtualAddress());

        // 6. 디스패치
        UINT numThreadGroups =
            static_cast<UINT>((scene._updateResource->_totalParticles.size() + 31) / 32); // 32개 스레드 그룹으로 나누기
        scene._commandList->Dispatch(numThreadGroups, 1, 1);

        computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(scene._renderResource->_simulationOutput.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS,
                                                                    D3D12_RESOURCE_STATE_COMMON);
        scene._commandList->ResourceBarrier(1, &computeOutputBarrior);
    }

}

void ParticleManager::DispatchRibbon(float deltaTime, std::string sceneName) 
{
    ParticleSceneResource& scene = _sceneResources[sceneName];

    {
        if (0 >= scene._updateResource->_ribbonTotalCount)
            return;
        CD3DX12_RESOURCE_BARRIER computeOutputBarrior =
            CD3DX12_RESOURCE_BARRIER::Transition(scene._renderResource->_ribbonSimulationOutput.Get(),
                                                 D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        scene._commandList->ResourceBarrier(1, &computeOutputBarrior);

        scene._commandList->SetComputeRootSignature(_computeSpriteRootSignature.Get());

        scene._commandList->SetComputeRootConstantBufferView(
            0, scene._renderResource->_mvpConstant->GetGPUVirtualAddress());
        scene._commandList->SetComputeRootShaderResourceView(
            1, scene._updateResource->_ribbonParticleInput->GetGPUVirtualAddress());
        scene._commandList->SetComputeRootShaderResourceView(
            2, scene._updateResource->_ribbonEmitterInfo->GetGPUVirtualAddress());
        scene._commandList->SetComputeRootUnorderedAccessView(
            3, scene._renderResource->_ribbonSimulationOutput->GetGPUVirtualAddress());

        // 6. 디스패치
        UINT numThreadGroups = static_cast<UINT>((scene._updateResource->_ribbonTotalParticles.size() + 31) /
                                                 32); // 32개 스레드 그룹으로 나누기
        scene._commandList->Dispatch(numThreadGroups, 1, 1);

        computeOutputBarrior =
            CD3DX12_RESOURCE_BARRIER::Transition(scene._renderResource->_ribbonSimulationOutput.Get(),
                                                 D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
        scene._commandList->ResourceBarrier(1, &computeOutputBarrior);
    }
}

void ParticleManager::UpdateAndCopyParticleResource(float deltaTime, ParticleUpdateResource* scene)
{
    //memcpy to upload heap
    {
            void* mappedData = nullptr;
        if (0 < scene->_totalCount)
        {
            scene->_particleInputUpload->Map(0, nullptr, &mappedData);
            memcpy(mappedData, scene->_totalParticles.data(), scene->_totalCount * sizeof(Particle));
            scene->_particleInputUpload->Unmap(0, nullptr);

            scene->_emitterInfoUpload->Map(0, nullptr, &mappedData);
            memcpy(mappedData, scene->_emitterMatrix.data(), scene->_emitterMatrix.size() * sizeof(EmitterInfo));
            scene->_emitterInfoUpload->Unmap(0, nullptr);
        }
        if (0 < scene->_ribbonTotalCount)
        {
            scene->_ribbonParticleInputUpload->Map(0, nullptr, &mappedData);
            memcpy(mappedData, scene->_ribbonTotalParticles.data(), scene->_ribbonTotalCount * sizeof(Particle));
            scene->_ribbonParticleInputUpload->Unmap(0, nullptr);

            scene->_ribbonEmitterInfoUpload->Map(0, nullptr, &mappedData);
            memcpy(mappedData, scene->_ribbonEmitterMatrix.data(),
                   scene->_ribbonEmitterMatrix.size() * sizeof(EmitterInfo));
            scene->_ribbonEmitterInfoUpload->Unmap(0, nullptr);
        }
        
    }
    //copy data from upload to default heap
    {
        CD3DX12_RESOURCE_BARRIER preCopyBarriers[] = {
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_particleInput.Get(), D3D12_RESOURCE_STATE_COMMON,
                                                 D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_emitterInfo.Get(), D3D12_RESOURCE_STATE_COMMON,
                                                 D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_ribbonParticleInput.Get(), D3D12_RESOURCE_STATE_COMMON,
                                                 D3D12_RESOURCE_STATE_COPY_DEST),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_ribbonEmitterInfo.Get(), D3D12_RESOURCE_STATE_COMMON,
                                                 D3D12_RESOURCE_STATE_COPY_DEST)};
        _computeCommandList->ResourceBarrier(_countof(preCopyBarriers), preCopyBarriers);

        UINT64 particleDataSize = scene->_totalParticles.size() * sizeof(Particle);
        _computeCommandList->CopyBufferRegion(scene->_particleInput.Get(),       // Dest
                                              0,                                 // DestOffset
                                              scene->_particleInputUpload.Get(), // Src
                                              0,                                 // SrcOffset
                                              particleDataSize                   // NumBytes
        );

        UINT64 emitterDataSize = scene->_emitterMatrix.size() * sizeof(EmitterInfo);
        _computeCommandList->CopyBufferRegion(scene->_emitterInfo.Get(), 0, scene->_emitterInfoUpload.Get(), 0,
                                              emitterDataSize);

        particleDataSize = scene->_ribbonTotalParticles.size() * sizeof(Particle);
        _computeCommandList->CopyBufferRegion(scene->_ribbonParticleInput.Get(),       // Dest
                                              0,                                       // DestOffset
                                              scene->_ribbonParticleInputUpload.Get(), // Src
                                              0,                                       // SrcOffset
                                              particleDataSize                         // NumBytes
        );

        emitterDataSize = scene->_ribbonEmitterMatrix.size() * sizeof(EmitterInfo);
        _computeCommandList->CopyBufferRegion(scene->_ribbonEmitterInfo.Get(), 0, scene->_ribbonEmitterInfoUpload.Get(),
                                              0, emitterDataSize);

        CD3DX12_RESOURCE_BARRIER postCopyBarriers[] = {
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_particleInput.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                 D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_emitterInfo.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                 D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_ribbonParticleInput.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                 D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
            CD3DX12_RESOURCE_BARRIER::Transition(scene->_ribbonEmitterInfo.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                                 D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)};
        _computeCommandList->ResourceBarrier(_countof(postCopyBarriers), postCopyBarriers);
    }

}

void ParticleManager::UpdateMvpConstant(float deltaTime, ParticleRenderResource* scene) 
{
    {
        SetCamera(scene->_name);
        // 3. MVP 상수 버퍼 업데이트
        MVPConstants mvpConstants;
        mvpConstants.ViewMatrix = _camera->GetViewMatrix().Transpose();
        Matrix viewrotinv       = _camera->GetViewMatrix();

        XMFLOAT3X3 rotV;
        XMStoreFloat3x3(&rotV, viewrotinv);

        // 2) 전치(transpose)하여 역회전 행렬 생성
        XMMATRIX Rv  = XMLoadFloat3x3(&rotV);
        XMMATRIX RvT = XMMatrixTranspose(Rv);

        // 3) SimpleMath::Matrix로 변환하여 반환

        XMStoreFloat4x4(&mvpConstants.ViewRotInvMatrix, RvT);

        mvpConstants.ViewRotInvMatrix = mvpConstants.ViewRotInvMatrix.Transpose();
        mvpConstants.ProjMatrix       = _camera->GetProjectionMatrix().Transpose();

        mvpConstants.CameraPos =
            Vector4(_camera->GetWorldMatrix()._41, _camera->GetWorldMatrix()._42, _camera->GetWorldMatrix()._43, 1);

        mvpConstants.deltaTime = deltaTime;

        void* mappedData       = nullptr;
        FAILED_CHECK_MESSAGE(scene->_mvpConstant->Map(0, nullptr, &mappedData), L"");
        memcpy(mappedData, &mvpConstants, sizeof(MVPConstants));
        scene->_mvpConstant->Unmap(0, nullptr);
    }


}

void ParticleManager::AddSceneResource(std::string_view sceneName)
{
    std::wstring wSceneName;
    wSceneName.assign(sceneName.begin(), sceneName.end());

    ParticleSceneResource newSceneResource;
    newSceneResource._name = sceneName;

    UINT particleInputSize           = _maxParticles * sizeof(Particle);
    UINT emitterInfoSize             = _maxEmitters * sizeof(EmitterInfo);
    UINT particleOutputSize          = _maxParticles * sizeof(ParticleOutput);
    UINT mvpConstantSize             = sizeof(MVPConstants);
    newSceneResource._updateResource = new ParticleUpdateResource();
    newSceneResource._updateResource->_name = sceneName;
    _particleUpdateResources.insert(newSceneResource._updateResource);

    newSceneResource._renderResource = new ParticleRenderResource();
    newSceneResource._renderResource->_name = sceneName;
    {
        {
            CreateStructuredBuffer(newSceneResource._updateResource->_particleInput,
                                   newSceneResource._updateResource->_particleInputUpload, particleInputSize,
                                   sizeof(Particle));
            auto inputname = (wSceneName + L" particle input");
            newSceneResource._updateResource->_particleInput->SetName(inputname.c_str());
            auto inputuploadname = (wSceneName + L" particle input upload");
            newSceneResource._updateResource->_particleInputUpload->SetName(inputuploadname.c_str());

            CreateStructuredBuffer(newSceneResource._updateResource->_ribbonParticleInput,
                                   newSceneResource._updateResource->_ribbonParticleInputUpload, particleInputSize,
                                   sizeof(Particle));
            auto ribboninputname = (wSceneName + L" ribbon particle input");
            newSceneResource._updateResource->_ribbonParticleInput->SetName(ribboninputname.c_str());
            auto ribboninputuploadname = (wSceneName + L" ribbon particle input upload");
            newSceneResource._updateResource->_ribbonParticleInputUpload->SetName(ribboninputuploadname.c_str());
        }
        {
            CreateStructuredBuffer(newSceneResource._updateResource->_emitterInfo,
                                   newSceneResource._updateResource->_emitterInfoUpload, emitterInfoSize,
                                   sizeof(EmitterInfo));

            auto emitterinfoname = (wSceneName + L" emitter info");
            newSceneResource._updateResource->_emitterInfo->SetName(emitterinfoname.c_str());

            auto emitterinfouploadname = (wSceneName + L" emitter info upload");
            newSceneResource._updateResource->_emitterInfoUpload->SetName(emitterinfouploadname.c_str());

            CreateStructuredBuffer(newSceneResource._updateResource->_ribbonEmitterInfo,
                                   newSceneResource._updateResource->_ribbonEmitterInfoUpload, emitterInfoSize,
                                   sizeof(EmitterInfo));

            auto ribbonemitterinfoname = (wSceneName + L" ribbon emitter info");
            newSceneResource._updateResource->_ribbonEmitterInfo->SetName(emitterinfoname.c_str());

            auto ribbonemitterinfouploadname = (wSceneName + L" ribbon emitter info upload");
            newSceneResource._updateResource->_ribbonEmitterInfoUpload->SetName(emitterinfouploadname.c_str());
        }

        {

            CreateUAVBuffer(newSceneResource._renderResource->_simulationOutput, particleOutputSize,
                            sizeof(ParticleOutput));
            auto outputname = (wSceneName + L" output");
            newSceneResource._renderResource->_simulationOutput->SetName(outputname.c_str());

            CreateUAVBuffer(newSceneResource._renderResource->_ribbonSimulationOutput, particleOutputSize,
                            sizeof(ParticleOutput));
            auto ribbonoutputname = (wSceneName + L" ribbon output");
            newSceneResource._renderResource->_ribbonSimulationOutput->SetName(outputname.c_str());
        }
        {

            CreateConstantBuffer(newSceneResource._renderResource->_mvpConstant, mvpConstantSize);
            auto mvpconstantsname = (wSceneName + L" mvp constants");
            newSceneResource._renderResource->_mvpConstant->SetName(mvpconstantsname.c_str());
        }
    }

    
    _sceneResources[std::string(sceneName)] = newSceneResource;
    InitializeComputeCommandObject(sceneName);

    _currentBufferIndex        = 0;
    _particleStride            = sizeof(Particle);
    _maxParticles              = _maxParticles;
    newSceneResource._updateResource->_totalParticles.resize(_maxParticles);
    newSceneResource._updateResource->_ribbonTotalParticles.resize(_maxParticles);
}

void ParticleManager::AddSceneResource(std::string_view sceneName, std::string_view sharedFrom)
{
    {
        std::wstring wSceneName;
        wSceneName.assign(sceneName.begin(), sceneName.end());

        ParticleSceneResource newSceneResource;
        newSceneResource._name = sceneName;


        UINT                   particleOutputSize = _maxParticles * sizeof(ParticleOutput);
        UINT                   mvpConstantSize    = sizeof(MVPConstants);
        ParticleSceneResource& srcScene           = _sceneResources[std::string(sharedFrom)];
        newSceneResource._updateResource          = srcScene._updateResource;
        newSceneResource._renderResource          = new ParticleRenderResource();
        newSceneResource._renderResource->_name   = sceneName;
        {
            {
                CreateUAVBuffer(newSceneResource._renderResource->_simulationOutput, particleOutputSize,
                                sizeof(ParticleOutput));
                auto outputname = (wSceneName + L" output");
                newSceneResource._renderResource->_simulationOutput->SetName(outputname.c_str());

                CreateUAVBuffer(newSceneResource._renderResource->_ribbonSimulationOutput, particleOutputSize,
                                sizeof(ParticleOutput));
                auto ribbonoutputname = (wSceneName + L" ribbon output");
                newSceneResource._renderResource->_ribbonSimulationOutput->SetName(outputname.c_str());
            }
            {
                CreateConstantBuffer(newSceneResource._renderResource->_mvpConstant, mvpConstantSize);
                auto mvpconstantsname = (wSceneName + L" mvp constants");
                newSceneResource._renderResource->_mvpConstant->SetName(mvpconstantsname.c_str());
            }
        }

        _sceneResources[std::string(sceneName)] = newSceneResource;
        InitializeComputeCommandObject(sceneName);

    }
}

void ParticleManager::RefreshCurrentEditorEffect()
{
    _editorCurrentEffect->FlushEmitters();
    _editorCurrentEffect->Reset();
    _editorCurrentEffect->Play();
    _editorRefreshFlag = false;
}
