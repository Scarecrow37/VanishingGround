#include "pch.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"

ParticleManager::ParticleManager() {}

ParticleManager::~ParticleManager()
{
    for (auto effect : _particleEffects)
    {
        if (nullptr!= effect)
            delete effect;
    }
    _particleEffects.clear();

    if (_editorCurrentEffect)
        delete _editorCurrentEffect;

    _totalParticles.clear();
    _emitterMatrix.clear();
    _activeEmitterAlbedos.clear();
    _activeEmitterNormals.clear();
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
    InitializeDescriptorHeap();   
}

ParticleEffect* ParticleManager::RegisterEffect()
{
    auto newEffect = new ParticleEffect();
    newEffect->Initialize(this);
    std::string name = "Effect" + std::to_string(nameingIndex++);
    newEffect->SetEffectName(name);

    _particleEffects.push_back(newEffect);
    return newEffect;
}

class ParticleEffect* ParticleManager::RegisterEffectOnEditor() 
{
    auto newEffect = new ParticleEffect();
    newEffect->Initialize(this);
    std::string name = "Effect" + std::to_string(nameingIndex++);
    newEffect->SetEffectName(name);
    _editorCurrentEffect = newEffect;
    return newEffect;
}

ParticleEmitter* ParticleManager::RegisterEmitter(class ParticleEffect* effect, SIZE_T maxParticles /*= 100000*/,
                                                  float emissionRate /*= 500.f*/, float emitterLifetime /*= 5.f*/,
                                                  LocationShape locatorShape /*= LocationShape::SPHERE*/,
                                                  Vector3       locationFactor /*= Vector3(1, 1, 1)*/,
                                                  ParticleType  particleType /*= ParticleType::SPRITE*/,
                                  std::wstring  meshspritePath /*= L""*/)
{
    auto newEmitter =
        effect->AddEmitter(maxParticles, emissionRate, emitterLifetime, locatorShape, locationFactor, particleType,meshspritePath);
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
    float delta = deltaTime *_deltaScale;
    if (nullptr == _currentRenderscene)
        return;
    
     
    //game view
    {

        for (auto effect : _particleEffects)
        {
            if (true == effect->GetActiveFlag())
            {
                effect->Update(delta);
            }
        }
        if (false == _particleEffects.empty())
        {
            CopyActiveParticles();
        }
        _computeAllocator->Reset();
        _computeCommandList->Reset(_computeAllocator.Get(), _computeSpritePSO.Get());
        DispatchParticleCompute(delta);

        for (auto effect : _particleEffects)
        {
            if (true == effect->GetActiveFlag())
            {
                effect->UpdateParticleLifeCycle(delta);
            }
        }
        UpdateEffectLifeCycle();
    }

    //particle editor
    {
        if (nullptr == _editorCurrentEffect)
        {
            _computeCommandList->Close();
            Global::commandController->ExecuteCommand(COMPUTE_QUEUE, _computeCommandList.Get());
            return;
        }
        _editorCurrentEffect->Update(delta);
        CopyActiveParticlesEditorMode();
        DispatchParticleComputeEditorMode(delta);

        _computeCommandList->Close();
        Global::commandController->ExecuteCommand(COMPUTE_QUEUE, _computeCommandList.Get());

        if (true == _editorCurrentEffect->GetActiveFlag())
        {
            _editorCurrentEffect->UpdateParticleLifeCycle(delta);
        }
        UpdateEditorLifeCycle();

        if (false == _editorCurrentEffect->GetActiveFlag() && true == _isAutoRefresh)
            RefreshEditor();

        if (true == _editorRefreshFlag)
            RefreshCurrentEditorEffect();
    }
}

void ParticleManager::UpdateEffectLifeCycle() 
{
    // erase_if 전에 메모리 해제
    for (auto it = _particleEffects.begin(); it != _particleEffects.end();)
    {
        if (true == (*it)->GetRemoveFlag())
         {
             delete *it; // 메모리 해제
             it = _particleEffects.erase(it);
         }
         else
         {
             ++it;
         }
     }

    for (auto newEffect : _particleEffects)
    {
        if (true == newEffect->GetPlayFlag())
        {
            newEffect->SetActiveFlag(true);
            newEffect->Reset();
            newEffect->SetPlayFlag(false);
 
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

void ParticleManager::SetCurrentRenderScene(RenderScene* renderScene) 
{
    _currentRenderscene = renderScene;
}

UINT ParticleManager::GetTotalCount() const
{
    if ("Game" == _currentRenderscene->_name || "Editor" == _currentRenderscene->_name)
        return _totalCount;
    else if ("ParticleEditor" == _currentRenderscene->_name)
        return _editorCount;
    else
        return 0;
}

std::vector<Texture*> ParticleManager::GetActiveAlbedos() const
{
    if ("Game" == _currentRenderscene->_name || "Editor" == _currentRenderscene->_name)
        return _activeEmitterAlbedos;
    else // ("ParticleEditor" == _currentRenderscene->_name)
        return _activeEditorAlbedos;
}

ID3D12Resource* ParticleManager::GetComputeOutputResource()
{
    if ("Editor" == _currentRenderscene->_name)
        return _particleOutputBuffer.Get();

    else if ("Game" == _currentRenderscene->_name)
        return _gameViewOutputBuffer.Get();

    else if ("ParticleEditor" == _currentRenderscene->_name)
        return _editorOutputBuffer.Get();
    else
        return nullptr;
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
        hr         = Global::device->GetDevice()->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(&_computeAllocator));
        FAILED_CHECK_MESSAGE(
            hr, L"ParticleManager::InitializeComputeCommandObject Global::device->GetDevice()->CreateCommandAllocator Failed");

        FAILED_CHECK_MESSAGE(Global::device->GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, _computeAllocator.Get(),
                                                                     nullptr,
                                                                     IID_PPV_ARGS(_computeCommandList.GetAddressOf())),
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
//    // axial billboard sprite particle compute shader
//    {
//
//        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES |
//                     D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
//
//#ifdef _DEBUG
//        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
//#endif
//
//        hr = D3DCompileFromFile(L"../Shaders/cs_compute_axial_sprite.hlsl", // HLSL 파일 경로
//                                nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
//                                "cs_main", // 셰이더 진입점
//                                "cs_5_1",  // 셰이더 모델
//                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
//                                _computeAxialSpriteShaderBlob.GetAddressOf(), error.GetAddressOf());
//
//        if (nullptr != error)
//        {
//            std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
//            GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
//        }
//
//        FAILED_CHECK_MESSAGE(hr, L"D3DCompileFromFile Failed");
//    }
//    // mesh particle compute shader
//    {
//
//        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES |
//                     D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
//
//#ifdef _DEBUG
//        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
//#endif
//
//        hr = D3DCompileFromFile(L"../Shaders/cs_compute_mesh.hlsl", // HLSL 파일 경로
//                                nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
//                                "cs_main", // 셰이더 진입점
//                                "cs_5_1",  // 셰이더 모델
//                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
//                                _computeMeshShaderBlob.GetAddressOf(), error.GetAddressOf());
//
//        if (nullptr != error)
//        {
//            std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
//            GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
//        }
//
//        FAILED_CHECK_MESSAGE(hr, L"D3DCompileFromFile Failed");
//    }
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
    // initialize mesh root signature;
    {

        //std::vector<D3D12_ROOT_PARAMETER> rootParameters;
        //rootParameters.resize(4);

        //rootParameters[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
        //rootParameters[0].Descriptor.ShaderRegister = 0; // b0
        //rootParameters[0].Descriptor.RegisterSpace  = 0;
        //rootParameters[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

        //// SRV (t0)
        //rootParameters[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
        //rootParameters[1].Descriptor.ShaderRegister = 0; // t0
        //rootParameters[1].Descriptor.RegisterSpace  = 0;
        //rootParameters[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

        //// SRV (t1)
        //rootParameters[2].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
        //rootParameters[2].Descriptor.ShaderRegister = 1; // t1
        //rootParameters[2].Descriptor.RegisterSpace  = 0;
        //rootParameters[2].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

        //// UAV (u0)
        //rootParameters[3].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
        //rootParameters[3].Descriptor.ShaderRegister = 0; // u0
        //rootParameters[3].Descriptor.RegisterSpace  = 0;
        //rootParameters[3].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

        //D3D12_ROOT_SIGNATURE_DESC rootSignDesc;
        //rootSignDesc.NumParameters     = (UINT)rootParameters.size();
        //rootSignDesc.pParameters       = rootParameters.data();
        //rootSignDesc.Flags             = D3D12_ROOT_SIGNATURE_FLAG_NONE;
        //rootSignDesc.NumStaticSamplers = 0;
        //rootSignDesc.pStaticSamplers   = nullptr;

        //ComPtr<ID3DBlob> serializedRootSig;
        //ComPtr<ID3DBlob> error;
        //HRESULT          hr = D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        //                                                  serializedRootSig.GetAddressOf(), error.GetAddressOf());
        //if (nullptr != error)
        //{
        //    std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
        //    GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        //}

        //FAILED_CHECK_MESSAGE(hr, L"");

        //ComPtr<ID3D12RootSignature> rootSignature;
        //hr = Global::device->GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
        //                                               serializedRootSig->GetBufferSize(),
        //                                               IID_PPV_ARGS(_computeMeshRootSignature.GetAddressOf()));
        //FAILED_CHECK_MESSAGE(hr, L"");
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
    // initialize axial sprite pipeline state object
    {
        //D3D12_COMPUTE_PIPELINE_STATE_DESC computePSODesc = {};
        //ZeroMemory(&computePSODesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
        //computePSODesc.CS             = {_computeAxialSpriteShaderBlob->GetBufferPointer(),
        //                                 _computeAxialSpriteShaderBlob->GetBufferSize()};
        //computePSODesc.pRootSignature = _computeSpriteRootSignature.Get();

        //HRESULT hr;
        //hr = Global::device->GetDevice()->CreateComputePipelineState(&computePSODesc,
        //                                                      IID_PPV_ARGS(_computeAxialSpritePSO.GetAddressOf()));
        //FAILED_CHECK_MESSAGE(hr, L"");
    }
    // initialize mesh pipeline state object
    {
        //D3D12_COMPUTE_PIPELINE_STATE_DESC computePSODesc = {};
        //ZeroMemory(&computePSODesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
        //computePSODesc.CS = {_computeMeshShaderBlob->GetBufferPointer(), _computeMeshShaderBlob->GetBufferSize()};
        //computePSODesc.pRootSignature = _computeMeshRootSignature.Get();

        //HRESULT hr;
        //hr = Global::device->GetDevice()->CreateComputePipelineState(&computePSODesc,
        //                                                      IID_PPV_ARGS(_computeMeshPSO.GetAddressOf()));
        //FAILED_CHECK_MESSAGE(hr, L"");
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

    _descriptorSize = Global::device->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CreateParticleResources();
}
void ParticleManager::CreateParticleResources()
{
    // 1. 파티클 입력 버퍼 (SRV - t0)
    UINT particleInputSize = _maxParticles * sizeof(Particle);
    CreateStructuredBuffer(_particleInputBuffer, _particleInputUploadBuffer, particleInputSize, sizeof(Particle));
    CreateStructuredBuffer(_editorParticleInputBuffer, _editorParticleInputUploadBuffer, particleInputSize,  sizeof(Particle));

    // 2. 에미터 정보 버퍼 (SRV - t1)
    UINT emitterInfoSize = _maxEmitters * sizeof(EmitterInfo);
    CreateStructuredBuffer(_emitterInfoBuffer, _emitterInfoUploadBuffer, emitterInfoSize, sizeof(EmitterInfo));
    CreateStructuredBuffer(_editorEmitterInfoBuffer, _editorEmitterInfoUploadBuffer, emitterInfoSize, sizeof(EmitterInfo));

    // 3. 파티클 출력 버퍼 (UAV - u0)
    UINT particleOutputSize = _maxParticles * sizeof(ParticleOutput);
    CreateUAVBuffer(_particleOutputBuffer, particleOutputSize, sizeof(ParticleOutput));
    _particleOutputBuffer->SetName(L"particle output");
    CreateUAVBuffer(_editorOutputBuffer, particleOutputSize, sizeof(ParticleOutput));
    _editorOutputBuffer->SetName(L"editor output");
    CreateUAVBuffer(_gameViewOutputBuffer, particleOutputSize, sizeof(ParticleOutput));
    _gameViewOutputBuffer->SetName(L"game view output");

    // 4. MVP 상수 버퍼 (CBV - b0)
    UINT mvpConstantSize = sizeof(MVPConstants); // 256바이트 정렬
    CreateConstantBuffer(_mvpConstantBuffer, mvpConstantSize);
    CreateConstantBuffer(_editorMvpConstantBuffer, mvpConstantSize);
    CreateConstantBuffer(_gameViewMvpConstantBuffer, mvpConstantSize);
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

    FAILED_CHECK_MESSAGE(Global::device->GetDevice()->CreateCommittedResource(&uploadProperty, D3D12_HEAP_FLAG_NONE,
                                                                       &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
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

void ParticleManager::CopyActiveParticles()
{
    _totalParticles.clear();
    _emitterMatrix.clear();
    _activeEmitterAlbedos.clear();
    UINT emitterIndex = 0;
    _totalCount       = 0;
    for (auto effect : _particleEffects)
    {
        if (true == effect->GetActiveFlag())
        {
            for (auto emitter : effect->GetEmitterList())
            {
                if (true == emitter->GetActiveFlag())
                {
                    if (ParticleType::SPRITE == emitter->_particleType)
                    {
                        _activeEmitterAlbedos.push_back(
                            static_cast<SpriteModule*>(emitter->_particleRenderModule)->GetAlbedoTexture());
                    }
                    _emitterMatrix.push_back({emitter->GetWorldMatrix().Transpose(), emitter->GetDragPoint(),
                                              emitter->GetDragForce(), emitter->GetVortexForce(),
                                              emitter->GetStartScale(), emitter->GetEndScale(),
                                              Vector4(emitter->GetStartColor().x, emitter->GetStartColor().y,
                                                      emitter->GetStartColor().z, emitter->GetStartOpacity()),
                                              Vector4(emitter->GetEndColor().x, emitter->GetEndColor().y,
                                                      emitter->GetEndColor().z, emitter->GetEndOpacity()),
                                              Vector4(emitter->GetParticleLifetime(), 0, 0, 0)});
                    auto& particlePool = emitter->GetParticlePool();
                    for (UINT i = 0; i < emitter->GetActiveParticleCount(); i++)
                    {
                        auto& particle = *particlePool[i];
                        particle.SetEmitterIndex(emitterIndex);
                        _totalParticles.push_back(particle);
                        _totalCount++;
                    }
                    emitterIndex++;
                }
            }
        }
    }
}
void ParticleManager::DispatchParticleCompute(float deltaTime)
{
    if (0 >= _totalCount)
        return;

    // 1. 리소스 업데이트
    UpdateParticleResources(deltaTime);

    // upload buffer -> default buf
    CopyFromUploadBuffer();

    // TODO:: JJW 이제 그래픽스 엔진은 에디터를 모름
    //if (IS_EDITOR)
    {
        CD3DX12_RESOURCE_BARRIER computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
            _particleOutputBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        _computeCommandList->ResourceBarrier(1, &computeOutputBarrior);

        _computeCommandList->SetComputeRootSignature(_computeSpriteRootSignature.Get());

        _computeCommandList->SetComputeRootConstantBufferView(0, _mvpConstantBuffer->GetGPUVirtualAddress());
        _computeCommandList->SetComputeRootShaderResourceView(1, _particleInputBuffer->GetGPUVirtualAddress());

        _computeCommandList->SetComputeRootShaderResourceView(2, _emitterInfoBuffer->GetGPUVirtualAddress());

        _computeCommandList->SetComputeRootUnorderedAccessView(3, _particleOutputBuffer->GetGPUVirtualAddress());

        // 6. 디스패치
        UINT numThreadGroups = static_cast<UINT>((_totalParticles.size() + 31) / 32); // 32개 스레드 그룹으로 나누기
        _computeCommandList->Dispatch(numThreadGroups, 1, 1);

        computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
            _particleOutputBuffer.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
        _computeCommandList->ResourceBarrier(1, &computeOutputBarrior);
    }

    {
        CD3DX12_RESOURCE_BARRIER computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
            _particleOutputBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
        _computeCommandList->ResourceBarrier(1, &computeOutputBarrior);

        _computeCommandList->SetComputeRootSignature(_computeSpriteRootSignature.Get());

        _computeCommandList->SetComputeRootConstantBufferView(0, _gameViewMvpConstantBuffer->GetGPUVirtualAddress());
        _computeCommandList->SetComputeRootShaderResourceView(1, _particleInputBuffer->GetGPUVirtualAddress());

        _computeCommandList->SetComputeRootShaderResourceView(2, _emitterInfoBuffer->GetGPUVirtualAddress());

        _computeCommandList->SetComputeRootUnorderedAccessView(3, _gameViewOutputBuffer->GetGPUVirtualAddress());

        // 6. 디스패치
        UINT numThreadGroups = static_cast<UINT>((_totalParticles.size() + 31) / 32); // 32개 스레드 그룹으로 나누기
        _computeCommandList->Dispatch(numThreadGroups, 1, 1);

        computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
            _particleOutputBuffer.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
        _computeCommandList->ResourceBarrier(1, &computeOutputBarrior);
    }

}
void ParticleManager::UpdateParticleResources(float deltaTime)
{
    // 1. 파티클 입력 버퍼 업데이트
    void* mappedData = nullptr;
    _particleInputUploadBuffer->Map(0, nullptr, &mappedData);
    memcpy(mappedData, _totalParticles.data(), _totalCount * sizeof(Particle));
    _particleInputUploadBuffer->Unmap(0, nullptr);

    _emitterInfoUploadBuffer->Map(0, nullptr, &mappedData);
    memcpy(mappedData, _emitterMatrix.data(), _emitterMatrix.size() * sizeof(EmitterInfo));
    _emitterInfoUploadBuffer->Unmap(0, nullptr);
    {
        SetCamera("Editor");
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
        //{
        //    mvpConstants.ViewMatrix       = Matrix::Identity;
        //    mvpConstants.ViewRotInvMatrix = Matrix::Identity;
        //    auto& mode                    = Global::device->GetMode();
        //    mvpConstants.ProjMatrix = Matrix::CreateOrthographic(1920.f, 1080.f,0.1f, 1000.f).Transpose();

        //    mvpConstants.CameraPos =
        //        Vector4(_camera->GetWorldMatrix()._41, _camera->GetWorldMatrix()._42, _camera->GetWorldMatrix()._43, 1);
        //}
        //mvpConstants.deltaTime = deltaTime;




        FAILED_CHECK_MESSAGE(_mvpConstantBuffer->Map(0, nullptr, &mappedData), L"");
        memcpy(mappedData, &mvpConstants, sizeof(MVPConstants));
        _mvpConstantBuffer->Unmap(0, nullptr);
    }
    {

        SetCamera("Game");
        // 3. MVP 상수 버퍼 업데이트
        MVPConstants gamveViewMvpConstants;
        gamveViewMvpConstants.ViewMatrix = _camera->GetViewMatrix().Transpose();
        Matrix viewrotinv                = _camera->GetViewMatrix();

        XMFLOAT3X3 rotV;
        XMStoreFloat3x3(&rotV, viewrotinv);

        // 2) 전치(transpose)하여 역회전 행렬 생성
        XMMATRIX Rv  = XMLoadFloat3x3(&rotV);
        XMMATRIX RvT = XMMatrixTranspose(Rv);

        // 3) SimpleMath::Matrix로 변환하여 반환

        XMStoreFloat4x4(&gamveViewMvpConstants.ViewRotInvMatrix, RvT);

        gamveViewMvpConstants.ViewRotInvMatrix = gamveViewMvpConstants.ViewRotInvMatrix.Transpose();
        gamveViewMvpConstants.ProjMatrix       = _camera->GetProjectionMatrix().Transpose();

        gamveViewMvpConstants.CameraPos =
            Vector4(_camera->GetWorldMatrix()._41, _camera->GetWorldMatrix()._42, _camera->GetWorldMatrix()._43, 1);

        gamveViewMvpConstants.deltaTime = deltaTime;

     /*           MVPConstants mvpConstants;
        {
            mvpConstants.ViewMatrix       = Matrix::Identity;
            mvpConstants.ViewRotInvMatrix = Matrix::Identity;
            auto& mode                    = Global::device->GetMode();
            mvpConstants.ProjMatrix       = Matrix::CreateOrthographic(1920.f, 1080.f, 0.1f, 1000.f).Transpose();

            mvpConstants.CameraPos =
                Vector4(_camera->GetWorldMatrix()._41, _camera->GetWorldMatrix()._42, _camera->GetWorldMatrix()._43, 1);
        }
        mvpConstants.deltaTime = deltaTime;
*/


        FAILED_CHECK_MESSAGE(_gameViewMvpConstantBuffer->Map(0, nullptr, &mappedData), L"");
        memcpy(mappedData, &gamveViewMvpConstants, sizeof(MVPConstants));
        //memcpy(mappedData, &mvpConstants, sizeof(MVPConstants));
        _gameViewMvpConstantBuffer->Unmap(0, nullptr);
    }

}
void ParticleManager::CopyFromUploadBuffer()
{
    // 3-1. 리소스 상태 전이 (COMMON → COPY_DEST)

    CD3DX12_RESOURCE_BARRIER preCopyBarriers[] = {
        CD3DX12_RESOURCE_BARRIER::Transition(_particleInputBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
                                             D3D12_RESOURCE_STATE_COPY_DEST),
        CD3DX12_RESOURCE_BARRIER::Transition(_emitterInfoBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
                                             D3D12_RESOURCE_STATE_COPY_DEST)};
    _computeCommandList->ResourceBarrier(_countof(preCopyBarriers), preCopyBarriers);

    // 3-2. 버퍼 복사 명령
    UINT64 particleDataSize = _totalParticles.size() * sizeof(Particle);
    _computeCommandList->CopyBufferRegion(_particleInputBuffer.Get(),       // Dest
                                          0,                                // DestOffset
                                          _particleInputUploadBuffer.Get(), // Src
                                          0,                                // SrcOffset
                                          particleDataSize                  // NumBytes
    );

    UINT64 emitterDataSize = _emitterMatrix.size() * sizeof(EmitterInfo);
    _computeCommandList->CopyBufferRegion(_emitterInfoBuffer.Get(), 0, _emitterInfoUploadBuffer.Get(), 0,
                                          emitterDataSize);

    // 3-3. 리소스 상태 전이 (COPY_DEST → SRV)
    CD3DX12_RESOURCE_BARRIER postCopyBarriers[] = {
        CD3DX12_RESOURCE_BARRIER::Transition(_particleInputBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                             D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
        CD3DX12_RESOURCE_BARRIER::Transition(_emitterInfoBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                             D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)};
    _computeCommandList->ResourceBarrier(_countof(postCopyBarriers), postCopyBarriers);
}

void ParticleManager::CopyActiveParticlesEditorMode()
{
    _editorTotalParticles.clear();
    _editorEmitterMatrix.clear();
    _activeEditorAlbedos.clear();
    UINT emitterIndex = 0;
    _editorCount       = 0;
    if (false == _editorCurrentEffect->GetActiveFlag())
        return;
    for (auto emitter : _editorCurrentEffect->GetEmitterList())
    {
        if (true == emitter->GetActiveFlag())
        {
            if (ParticleType::SPRITE == emitter->_particleType)
            {
                _activeEditorAlbedos.push_back(
                    static_cast<SpriteModule*>(emitter->_particleRenderModule)->GetAlbedoTexture());
            }
            _editorEmitterMatrix.push_back({emitter->GetWorldMatrix().Transpose(), emitter->GetDragPoint(),
                                            emitter->GetDragForce(), emitter->GetVortexForce(),
                                            emitter->GetStartScale(), emitter->GetEndScale(),
                                            Vector4(emitter->GetStartColor().x, emitter->GetStartColor().y,
                                                    emitter->GetStartColor().z, emitter->GetStartOpacity()),
                                            Vector4(emitter->GetEndColor().x, emitter->GetEndColor().y,
                                                    emitter->GetEndColor().z, emitter->GetEndOpacity()),
                                            Vector4(emitter->GetParticleLifetime(), 0, 0, 0)});
            auto& particlePool = emitter->GetParticlePool();
            for (UINT i = 0; i < emitter->GetActiveParticleCount(); i++)
            {

                auto& particle = *particlePool[i];
                particle.SetEmitterIndex(emitterIndex);
                _editorTotalParticles.push_back(particle);
                _editorCount++;
            }
            emitterIndex++;
        }
    }
}
void ParticleManager::DispatchParticleComputeEditorMode(float deltaTime) 
{
    if (0 >= _editorCount)
        return;

    // 1. 리소스 업데이트
    UpdateParticleResourcesEditorMode(deltaTime);

    // upload buffer -> default buf
    CopyFromUploadBufferEditorMode();

    CD3DX12_RESOURCE_BARRIER computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        _editorOutputBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _computeCommandList->ResourceBarrier(1, &computeOutputBarrior);

    _computeCommandList->SetComputeRootSignature(_computeSpriteRootSignature.Get());
    _computeCommandList->SetComputeRootConstantBufferView(0, _editorMvpConstantBuffer->GetGPUVirtualAddress());
    _computeCommandList->SetComputeRootShaderResourceView(1, _editorParticleInputBuffer->GetGPUVirtualAddress());
    _computeCommandList->SetComputeRootShaderResourceView(2, _editorEmitterInfoBuffer->GetGPUVirtualAddress());
    _computeCommandList->SetComputeRootUnorderedAccessView(3, _editorOutputBuffer->GetGPUVirtualAddress());

    UINT numThreadGroups = static_cast<UINT>((_editorTotalParticles.size() + 31) / 32); // 32개 스레드 그룹으로 나누기
    _computeCommandList->Dispatch(numThreadGroups, 1, 1);

    computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        _editorOutputBuffer.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
    _computeCommandList->ResourceBarrier(1, &computeOutputBarrior);


}
void ParticleManager::UpdateParticleResourcesEditorMode(float deltaTime) 
{

    // 1. 파티클 입력 버퍼 업데이트
    void* mappedData = nullptr;
    _editorParticleInputUploadBuffer->Map(0, nullptr, &mappedData);
    memcpy(mappedData, _editorTotalParticles.data(), _editorCount * sizeof(Particle));
    _editorParticleInputUploadBuffer->Unmap(0, nullptr);

    _editorEmitterInfoUploadBuffer->Map(0, nullptr, &mappedData);
    memcpy(mappedData, _editorEmitterMatrix.data(), _editorEmitterMatrix.size() * sizeof(EmitterInfo));
    _editorEmitterInfoUploadBuffer->Unmap(0, nullptr);


    SetCamera("ParticleEditor");
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


    //    MVPConstants mvpConstants;
    //{
    //    mvpConstants.ViewMatrix       = Matrix::Identity;
    //    mvpConstants.ViewRotInvMatrix = Matrix::Identity;
    //    auto& mode                    = Global::device->GetMode();
    //    mvpConstants.ProjMatrix       = Matrix::CreateOrthographic(1920.f, 1080.f, 0.1f, 1000.f).Transpose();

    //    mvpConstants.CameraPos =
    //        Vector4(_camera->GetWorldMatrix()._41, _camera->GetWorldMatrix()._42, _camera->GetWorldMatrix()._43, 1);

    //}
    mvpConstants.deltaTime = deltaTime;

    FAILED_CHECK_MESSAGE(_editorMvpConstantBuffer->Map(0, nullptr, &mappedData), L"");
    memcpy(mappedData, &mvpConstants, sizeof(MVPConstants));
    _editorMvpConstantBuffer->Unmap(0, nullptr);


}
void ParticleManager::CopyFromUploadBufferEditorMode() 
{
    // 3-1. 리소스 상태 전이 (COMMON → COPY_DEST)

    CD3DX12_RESOURCE_BARRIER preCopyBarriers[] = {
        CD3DX12_RESOURCE_BARRIER::Transition(_editorParticleInputBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
                                             D3D12_RESOURCE_STATE_COPY_DEST),
        CD3DX12_RESOURCE_BARRIER::Transition(_editorEmitterInfoBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
                                             D3D12_RESOURCE_STATE_COPY_DEST)};
    _computeCommandList->ResourceBarrier(_countof(preCopyBarriers), preCopyBarriers);

    // 3-2. 버퍼 복사 명령
    UINT64 particleDataSize = _editorTotalParticles.size() * sizeof(Particle);
    _computeCommandList->CopyBufferRegion(_editorParticleInputBuffer.Get(), // Dest
                                          0,                                // DestOffset
                                          _editorParticleInputUploadBuffer.Get(), // Src
                                          0,                                // SrcOffset
                                          particleDataSize                  // NumBytes
    );

    UINT64 emitterDataSize = _editorEmitterMatrix.size() * sizeof(EmitterInfo);
    _computeCommandList->CopyBufferRegion(_editorEmitterInfoBuffer.Get(), 0, _editorEmitterInfoUploadBuffer.Get(), 0,
                                          emitterDataSize);

    // 3-3. 리소스 상태 전이 (COPY_DEST → SRV)
    CD3DX12_RESOURCE_BARRIER postCopyBarriers[] = {
        CD3DX12_RESOURCE_BARRIER::Transition(_editorParticleInputBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                             D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE),
        CD3DX12_RESOURCE_BARRIER::Transition(_editorEmitterInfoBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST,
                                             D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE)};
    _computeCommandList->ResourceBarrier(_countof(postCopyBarriers), postCopyBarriers);


}

void ParticleManager::RefreshCurrentEditorEffect() 
{
    // TODO JJW
    //if (!Global::isEditorMode)
    //    return;

    _editorCurrentEffect->FlushEmitters();
    _editorCurrentEffect->Reset();
    _editorCurrentEffect->Play();
    _editorRefreshFlag = false;

}

