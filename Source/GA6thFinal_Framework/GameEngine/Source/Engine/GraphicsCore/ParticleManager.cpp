#include "pch.h"
#include "ParticleEffect.h"
#include "ParticleEmitter.h"
#include "ParticleManager.h"

ParticleManager::ParticleManager() {}
void ParticleManager::SetCamera(std::string_view viewName)
{
    _camera = UmRenderer.GetCamera(viewName);
}
void ParticleManager::SetCamera(std::shared_ptr<Camera> camera)
{
    _camera = camera;
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
    IntializeGraphicsCommandObject();
    InitializeDescriptorHeap();

    
  



}
ParticleEffect* ParticleManager::RegisterEffect()
{
    auto newEffect = new ParticleEffect();
    newEffect->Initialize(this);
    _pariticleEffects.push_back(newEffect);
    return newEffect;
}
ParticleEmitter* ParticleManager::RegisterEmitter(class ParticleEffect* effect, SIZE_T maxParticles /*= 100000*/,
                                                  float emissionRate /*= 500.f*/, float emitterLifetime /*= 5.f*/,
                                                  LocationShape locatorShape /*= LocationShape::SPHERE*/,
                                                  Vector3       locationFactor /*= Vector3(1, 1, 1)*/)
{
    auto newEmitter = effect->AddEmitter(maxParticles, emissionRate, emitterLifetime, locatorShape, locationFactor);
    return newEmitter;
}
void ParticleManager::Update(const float deltaTime)
{
    float delta = deltaTime;
    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Space))
    {
        pauseFlag = false == pauseFlag;
    }

    if (true == pauseFlag)
    {
        delta = 0;
    }

    for (auto effect : _pariticleEffects)
    {
        effect->Update(delta);
    }
    // copy active particle data
    {
        CopyActiveParticles();
    }

    // dispatch particle compute shader
    {
        DispatchParticleCompute(delta);
    }



    // update particle lifecycle
    for (auto effect : _pariticleEffects)
    {
        if (true == effect->GetActiveFlag())
        {
            effect->UpdateParticleLifeCycle(delta);
        }
    }
}
void ParticleManager::ResetRenderCommandObject()
{
    _renderAllocator->Reset();
    _renderCommandList->Reset(_renderAllocator.Get(), nullptr);
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
        hr = UmDevice.GetDevice()->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(&_computeAllocator));
        FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeComputeCommandObject UmDevice.GetDevice()->CreateCommandAllocator Failed");


        FAILED_CHECK_MESSAGE(
            UmDevice.GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, _computeAllocator.Get(), nullptr,
                                                        IID_PPV_ARGS(_computeCommandList.GetAddressOf())),
                L"");
        _computeCommandList->Close();
    }

}

void ParticleManager::IntializeGraphicsCommandObject()
{
    auto device = UmDevice.GetDevice();

    D3D12_COMMAND_QUEUE_DESC desc{
        .Type     = D3D12_COMMAND_LIST_TYPE_DIRECT,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
    };

    FAILED_CHECK_MESSAGE(device->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(_renderAllocator.GetAddressOf())),
                         L"ParticleManager::IntializeGraphicsCommandObject() device->CreateCommandAllocator Failed");
    FAILED_CHECK_MESSAGE(device->CreateCommandList(desc.NodeMask, desc.Type, _renderAllocator.Get(), nullptr,
                                                 IID_PPV_ARGS(_renderCommandList.GetAddressOf())),
                         L"ParticleManager::IntializeGraphicsCommandObject() device->CreateCommandList Failed");
    _renderCommandList->SetName(L"particle render commandlist");
    _renderCommandList->Close();
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
    // axial billboard sprite particle compute shader
    {

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES |
                     D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

#ifdef _DEBUG
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        hr = D3DCompileFromFile(L"../Shaders/cs_compute_axial_sprite.hlsl", // HLSL 파일 경로
                                nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                "cs_main", // 셰이더 진입점
                                "cs_5_1",  // 셰이더 모델
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
                                _computeAxialSpriteShaderBlob.GetAddressOf(), error.GetAddressOf());

        if (nullptr != error)
        {
            std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
            GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }

        FAILED_CHECK_MESSAGE(hr, L"D3DCompileFromFile Failed");
    }
    // mesh particle compute shader
    {

        UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES |
                     D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

#ifdef _DEBUG
        flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        hr = D3DCompileFromFile(L"../Shaders/cs_compute_mesh.hlsl", // HLSL 파일 경로
                                nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                                "cs_main", // 셰이더 진입점
                                "cs_5_1",  // 셰이더 모델
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
                                _computeMeshShaderBlob.GetAddressOf(), error.GetAddressOf());

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

        FAILED_CHECK_MESSAGE(hr,L"ParticleManager::InitializeParticleComputeRootSignature D3D12SerializeRootSignature Failed");

        ComPtr<ID3D12RootSignature> rootSignature;
        hr = UmDevice.GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
                                                       serializedRootSig->GetBufferSize(),
                                                       IID_PPV_ARGS(_computeSpriteRootSignature.GetAddressOf()));
        FAILED_CHECK_MESSAGE(hr,L"ParticleManager::InitializeParticleComputeRootSignature CreateRootSignature Failed");
    }
    // initialize mesh root signature;
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

        FAILED_CHECK_MESSAGE(hr,L"");

        ComPtr<ID3D12RootSignature> rootSignature;
        hr = UmDevice.GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
                                                       serializedRootSig->GetBufferSize(),
                                                       IID_PPV_ARGS(_computeMeshRootSignature.GetAddressOf()));
        FAILED_CHECK_MESSAGE(hr,L"");
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
        hr = UmDevice.GetDevice()->CreateComputePipelineState(&computePSODesc,
                                                              IID_PPV_ARGS(_computeSpritePSO.GetAddressOf()));
        FAILED_CHECK_MESSAGE(hr,L"");
    }
    // initialize axial sprite pipeline state object
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC computePSODesc = {};
        ZeroMemory(&computePSODesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
        computePSODesc.CS             = {_computeAxialSpriteShaderBlob->GetBufferPointer(),
                                         _computeAxialSpriteShaderBlob->GetBufferSize()};
        computePSODesc.pRootSignature = _computeSpriteRootSignature.Get();

        HRESULT hr;
        hr = UmDevice.GetDevice()->CreateComputePipelineState(&computePSODesc,
                                                              IID_PPV_ARGS(_computeAxialSpritePSO.GetAddressOf()));
        FAILED_CHECK_MESSAGE(hr,L"");
    }
    // initialize mesh pipeline state object
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC computePSODesc = {};
        ZeroMemory(&computePSODesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
        computePSODesc.CS = {_computeMeshShaderBlob->GetBufferPointer(), _computeMeshShaderBlob->GetBufferSize()};
        computePSODesc.pRootSignature = _computeMeshRootSignature.Get();

        HRESULT hr;
        hr = UmDevice.GetDevice()->CreateComputePipelineState(&computePSODesc,
                                                              IID_PPV_ARGS(_computeMeshPSO.GetAddressOf()));
        FAILED_CHECK_MESSAGE(hr,L"");
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
        UmDevice.GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_cbvSrvUavHeap.GetAddressOf())), L"");

    _descriptorSize = UmDevice.GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CreateParticleResources();
}
void ParticleManager::CreateParticleResources()
{
    // 1. 파티클 입력 버퍼 (SRV - t0)
    UINT particleInputSize = _maxParticles * sizeof(Particle);
    CreateStructuredBuffer(_particleInputBuffer, _particleInputUploadBuffer, particleInputSize, sizeof(Particle));

    // 2. 에미터 정보 버퍼 (SRV - t1)
    UINT emitterInfoSize = _maxEmitters * sizeof(EmitterInfo);
    CreateStructuredBuffer(_emitterInfoBuffer, _emitterInfoUploadBuffer, emitterInfoSize, sizeof(EmitterInfo));

    // 3. 파티클 출력 버퍼 (UAV - u0)
    UINT particleOutputSize = _maxParticles * sizeof(ParticleOutput);
    CreateUAVBuffer(_particleOutputBuffer, particleOutputSize, sizeof(ParticleOutput));
    _particleOutputBuffer->SetName(L"particle output");
    // 4. MVP 상수 버퍼 (CBV - b0)
    UINT mvpConstantSize = sizeof(MVPConstants); // 256바이트 정렬
    CreateConstantBuffer(_mvpConstantBuffer, mvpConstantSize);
    // 디스크립터 생성
    CreateDescriptors();
}
void ParticleManager::CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, ComPtr<ID3D12Resource>& uploadResource,
                                             UINT bufferSize, UINT stride)
{
    // 기본 버퍼 생성 (GPU 전용)
    D3D12_RESOURCE_DESC bufferDesc      = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    auto                defaultProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    FAILED_CHECK_MESSAGE(UmDevice.GetDevice()->CreateCommittedResource(&defaultProperty, D3D12_HEAP_FLAG_NONE,
                                                                     &bufferDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resource)),
                         L"");

    // 업로드 버퍼 생성 (CPU->GPU 전송용)
    auto uploadProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    FAILED_CHECK_MESSAGE(UmDevice.GetDevice()->CreateCommittedResource(&uploadProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc,
                                                                     D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadResource)),
                         L"");
}
void ParticleManager::CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride)
{
    // uav로 쓰고 srv로 읽기용
    D3D12_RESOURCE_DESC bufferDesc      = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    auto                defaultProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    FAILED_CHECK_MESSAGE(UmDevice.GetDevice()->CreateCommittedResource(&defaultProperty, D3D12_HEAP_FLAG_NONE,
                                                                     &bufferDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resource)),
                         L"");
}
void ParticleManager::CreateUAVBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride)
{
    // UAV 버퍼 생성 (GPU 읽기/쓰기)
    D3D12_RESOURCE_DESC bufferDesc =
        CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    auto defaultProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    FAILED_CHECK_MESSAGE(UmDevice.GetDevice()->CreateCommittedResource(&defaultProperty, D3D12_HEAP_FLAG_NONE,
                                                                     &bufferDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resource)),
                         L"");
}
void ParticleManager::CreateConstantBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize)
{
    // 상수 버퍼는 항상 256바이트 정렬되어야 함
    UINT alignedBufferSize = (bufferSize + 255) & ~255;

    D3D12_RESOURCE_DESC bufferDesc      = CD3DX12_RESOURCE_DESC::Buffer(alignedBufferSize);

    // 2. Upload Heap에 업로드 버퍼 생성 (CPU 접근 가능)
    auto uploadProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    FAILED_CHECK_MESSAGE(
        UmDevice.GetDevice()->CreateCommittedResource(&uploadProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc,
                                                      D3D12_RESOURCE_STATE_GENERIC_READ, // Upload Heap 필수 상태
                                                      nullptr, IID_PPV_ARGS(&resource)), L"");




}
void ParticleManager::CreateDescriptors()
{
    // 디스크립터 핸들 가져오기
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(_cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart());

    // 1. MVP 상수 버퍼 뷰 (b0)
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = _mvpConstantBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes    = sizeof(MVPConstants);
    UmDevice.GetDevice()->CreateConstantBufferView(&cbvDesc, handle);

    // 2. 파티클 입력 버퍼 뷰 (t0)
    handle.Offset(1, _descriptorSize);
    D3D12_SHADER_RESOURCE_VIEW_DESC particleInputSrvDesc{};
    particleInputSrvDesc.Format                     = DXGI_FORMAT_UNKNOWN;
    particleInputSrvDesc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
    particleInputSrvDesc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    particleInputSrvDesc.Buffer.FirstElement        = 0;
    particleInputSrvDesc.Buffer.NumElements         = _maxParticles;
    particleInputSrvDesc.Buffer.StructureByteStride = sizeof(Particle);
    particleInputSrvDesc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
    UmDevice.GetDevice()->CreateShaderResourceView(_particleInputBuffer.Get(), &particleInputSrvDesc, handle);

    // 3. 에미터 정보 버퍼 뷰 (t1)
    handle.Offset(1, _descriptorSize);
    D3D12_SHADER_RESOURCE_VIEW_DESC emitterInfoSrvDesc{};
    emitterInfoSrvDesc.Format                     = DXGI_FORMAT_UNKNOWN;
    emitterInfoSrvDesc.ViewDimension              = D3D12_SRV_DIMENSION_BUFFER;
    emitterInfoSrvDesc.Shader4ComponentMapping    = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    emitterInfoSrvDesc.Buffer.FirstElement        = 0;
    emitterInfoSrvDesc.Buffer.NumElements         = _maxEmitters;
    emitterInfoSrvDesc.Buffer.StructureByteStride = sizeof(EmitterInfo);
    emitterInfoSrvDesc.Buffer.Flags               = D3D12_BUFFER_SRV_FLAG_NONE;
    UmDevice.GetDevice()->CreateShaderResourceView(_emitterInfoBuffer.Get(), &emitterInfoSrvDesc, handle);

    // 4. 파티클 출력 버퍼 뷰 (u0)
    handle.Offset(1, _descriptorSize);
    D3D12_UNORDERED_ACCESS_VIEW_DESC particleOutputUavDesc{};
    particleOutputUavDesc.Format                      = DXGI_FORMAT_UNKNOWN;
    particleOutputUavDesc.ViewDimension               = D3D12_UAV_DIMENSION_BUFFER;
    particleOutputUavDesc.Buffer.FirstElement         = 0;
    particleOutputUavDesc.Buffer.NumElements          = _maxParticles;
    particleOutputUavDesc.Buffer.StructureByteStride  = sizeof(ParticleOutput);
    particleOutputUavDesc.Buffer.CounterOffsetInBytes = 0;
    particleOutputUavDesc.Buffer.Flags                = D3D12_BUFFER_UAV_FLAG_NONE;
    UmDevice.GetDevice()->CreateUnorderedAccessView(_particleOutputBuffer.Get(), nullptr, &particleOutputUavDesc,
                                                    handle);
}

void ParticleManager::CopyActiveParticles() 
{
    _totalParticles.clear();
    _emitterMatrix.clear();
    _activeEmitterAlbedos.clear();
    UINT emitterIndex = 0;
    _totalCount       = 0;
    for (auto effect : _pariticleEffects)
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
                    _emitterMatrix.push_back(
                        {emitter->GetWorldMatrix().Transpose(), emitter->GetDragPoint(), emitter->GetDragForce()});
                    auto& particlePool = emitter->GetParticlePool();
                    for (UINT i = 0; i < emitter->GetActiveParticleCount(); i++)
                    {

                        auto& particle = *particlePool[i];
                        particle.SetEmitterIndex(emitterIndex);
                        _totalParticles.push_back(particle);
                    }
                    _totalCount += emitter->GetActiveParticleCount() + 1;
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


    
        _computeAllocator->Reset();
    _computeCommandList->Reset(_computeAllocator.Get(), _computeSpritePSO.Get());

    // 1. 리소스 업데이트
    UpdateParticleResources(deltaTime);



    // upload buffer -> default buf
    CopyFromUploadBuffer();

    CD3DX12_RESOURCE_BARRIER computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        _particleOutputBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _computeCommandList->ResourceBarrier(1, &computeOutputBarrior);

    // 3. 디스크립터 힙 설정
    ID3D12DescriptorHeap* heaps[] = {_cbvSrvUavHeap.Get()};
    _computeCommandList->SetDescriptorHeaps(_countof(heaps), heaps);

    // 4. 루트 시그니처 설정
    _computeCommandList->SetComputeRootSignature(_computeSpriteRootSignature.Get());

    // 5. 루트 파라미터 설정 (디스크립터 테이블 대신 개별 디스크립터 사용)
    CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(_cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart());
    _computeCommandList->SetComputeRootConstantBufferView(0, _mvpConstantBuffer->GetGPUVirtualAddress());

    CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle1 = cbvHandle;
    srvHandle1.Offset(1, _descriptorSize);
    _computeCommandList->SetComputeRootShaderResourceView(1, _particleInputBuffer->GetGPUVirtualAddress());

    CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle2 = srvHandle1;
    srvHandle2.Offset(1, _descriptorSize);
    _computeCommandList->SetComputeRootShaderResourceView(2, _emitterInfoBuffer->GetGPUVirtualAddress());

    CD3DX12_GPU_DESCRIPTOR_HANDLE uavHandle = srvHandle2;
    uavHandle.Offset(1, _descriptorSize);
    _computeCommandList->SetComputeRootUnorderedAccessView(3, _particleOutputBuffer->GetGPUVirtualAddress());

    // 6. 디스패치
    UINT numThreadGroups = static_cast<UINT>((_totalParticles.size() + 31) / 32); // 32개 스레드 그룹으로 나누기
    _computeCommandList->Dispatch(numThreadGroups, 1, 1);

    computeOutputBarrior = CD3DX12_RESOURCE_BARRIER::Transition(
        _particleOutputBuffer.Get(), D3D12_RESOURCE_STATE_UNORDERED_ACCESS, D3D12_RESOURCE_STATE_COMMON);
    _computeCommandList->ResourceBarrier(1, &computeOutputBarrior);



    _computeCommandList->Close();
    UmDevice.RegisterCommand(_computeCommandList.Get(), PARTICLE_COMPUTE_LIST);


    // 7. 커맨드 리스트 종료 및 실행
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

    // 3. MVP 상수 버퍼 업데이트
    MVPConstants mvpConstants;
    mvpConstants.ViewMatrix           = _camera->GetViewMatrix().Transpose();
    Matrix viewrotinv = _camera->GetViewMatrix();

      XMFLOAT3X3 rotV;
        XMStoreFloat3x3(&rotV, viewrotinv);

    // 2) 전치(transpose)하여 역회전 행렬 생성
    XMMATRIX Rv  = XMLoadFloat3x3(&rotV);
    XMMATRIX RvT = XMMatrixTranspose(Rv);

    // 3) SimpleMath::Matrix로 변환하여 반환

    XMStoreFloat4x4(&mvpConstants.ViewRotInvMatrix, RvT);

    //mvpConstants.ViewRotInvMatrix = mvpConstants.ViewRotInvMatrix.Transpose();
    mvpConstants.ProjMatrix = _camera->GetProjectionMatrix().Transpose();

    mvpConstants.CameraPos =
        Vector4(_camera->GetWorldMatrix()._41, _camera->GetWorldMatrix()._42, _camera->GetWorldMatrix()._43, 1);

    float currentTime = UmTime.Time();
    float delta       = currentTime - lastFrameTime;
    lastFrameTime     = currentTime;

    // 컴퓨트 셰이더 디스패치
    mvpConstants.deltaTime = delta;

    FAILED_CHECK_MESSAGE(_mvpConstantBuffer->Map(0, nullptr, &mappedData),L"");
    memcpy(mappedData, &mvpConstants, sizeof(MVPConstants));
    _mvpConstantBuffer->Unmap(0, nullptr);
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
//===============================================================

