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

        // Radix Sort 초기화 추가
    InitializeRadixSortShaders();
    InitializeRadixSortRootSignature();
    InitializeRadixSortPSO();
    CreateRadixSortResources();


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
    if (ImGui::IsKeyDown(ImGuiKey::ImGuiKey_Space))
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
    _totalParticles.clear();
    _emitterMatrix.clear();
    _activeEmitterAlbedos.clear();
    UINT emitterIndex = 0;
    _totalCount       = 0;
    for (auto effect : _pariticleEffects)
    {
        //if (true == effect->GetActiveFlag())
        {
            for (auto emitter : effect->GetEmitterList())
            {
                //if (true == emitter->GetActiveFlag())
                {
                    if (ParticleType::SPRITE == emitter->_particleType)
                    {
                        _activeEmitterAlbedos.push_back(
                            static_cast<SpriteModule*>(emitter->_particleRenderModule)->GetAlbedoTexture());
                    }
                    _emitterMatrix.push_back({emitter->GetWorldMatrix().Transpose()});
                    auto& particlePool = emitter->GetParticlePool();
                    for (int i = 0; i < emitter->GetActiveParticleCount(); i++)
                    {

                        auto particle = *particlePool[i];
                        particle.SetEmitterIndex(emitterIndex);
                        _totalParticles.push_back(particle);
                        //_totalCount++;
                    }
                    _totalCount += emitter->GetActiveParticleCount()+1;
                    emitterIndex++;
                }
            }
        }
    }

    //simulate

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
            FAILED_CHECK_MESSAGE(hr, L"ParticleManager::InitializeComputeCommandObject UmDevice.GetDevice()->CreateCommandAllocator Failed",);


        FAILED_CHECK_MESSAGE(
            UmDevice.GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, _computeAllocator.Get(), nullptr,
                                                        IID_PPV_ARGS(_computeCommandList.GetAddressOf())),
                L"");
        _computeCommandList->Close();
    }
    {
        FAILED_CHECK_MESSAGE(UmDevice.GetDevice()->CreateCommandAllocator(
                                 desc.Type, IID_PPV_ARGS(_depthExtractAllocator.GetAddressOf())),
                             L"");
        FAILED_CHECK_MESSAGE(UmDevice.GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, _depthExtractAllocator.Get(),
                                                                   nullptr,
                                                    IID_PPV_ARGS(_depthExtractCommandList.GetAddressOf())),
            L"");
        _depthExtractCommandList->Close();
    }
   
    {
        FAILED_CHECK_MESSAGE(
            UmDevice.GetDevice()->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(_radixSortAllocator.GetAddressOf())),
            L"");
        FAILED_CHECK_MESSAGE(UmDevice.GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, _radixSortAllocator.Get(),
                                                                   nullptr,
                                                    IID_PPV_ARGS(_radixSortCommandList.GetAddressOf())),
            L"");
        _radixSortCommandList->Close();
    }
    {
        FAILED_CHECK_MESSAGE(
            UmDevice.GetDevice()->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(_reorderAllocator.GetAddressOf())),
            L"");
        FAILED_CHECK_MESSAGE(UmDevice.GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, _reorderAllocator.Get(),
                                                                   nullptr,
                                                                     IID_PPV_ARGS(_reorderCommandList.GetAddressOf())),
                             L"");
        _reorderCommandList->Close();
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

        FAILED_CHECK_MESSAGE(hr, L"D3DCompileFromFile Failed",);
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

        FAILED_CHECK_MESSAGE(hr, L"D3DCompileFromFile Failed",);
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

        FAILED_CHECK_MESSAGE(hr, L"D3DCompileFromFile Failed",);
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
        rootSignDesc.NumParameters     = rootParameters.size();
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

        FAILED_CHECK_MESSAGE(hr,L"D3D12SerializeRootSignature Failed",);

        ComPtr<ID3D12RootSignature> rootSignature;
        hr = UmDevice.GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
                                                       serializedRootSig->GetBufferSize(),
                                                       IID_PPV_ARGS(_computeSpriteRootSignature.GetAddressOf()));
        FAILED_CHECK_MESSAGE(hr,L"CreateRootSignature Failed",);
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
        rootSignDesc.NumParameters     = rootParameters.size();
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
    UINT numThreadGroups = (_totalParticles.size() + 31) / 32; // 32개 스레드 그룹으로 나누기
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
    mvpConstants.ViewRotInvMatrix     = _camera->GetViewMatrix().Transpose();
    mvpConstants.ViewRotInvMatrix._14 = 0.0f;
    mvpConstants.ViewRotInvMatrix._24 = 0.0f;
    mvpConstants.ViewRotInvMatrix._34 = 0.0f;
    mvpConstants.ViewRotInvMatrix._41 = 0.0f;
    mvpConstants.ViewRotInvMatrix._42 = 0.0f;
    mvpConstants.ViewRotInvMatrix._43 = 0.0f;
    mvpConstants.ViewRotInvMatrix._44 = 1.0f;
    mvpConstants.ViewRotInvMatrix     = mvpConstants.ViewRotInvMatrix.Invert();

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
void ParticleManager::InitializeRadixSortShaders() 
{
    HRESULT          hr = S_OK;
    ComPtr<ID3DBlob> error;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;
#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    // 깊이 추출 셰이더 (ParticleOutput에서 FinalMatrix._43 추출)
    hr = D3DCompileFromFile(L"../Shaders/cs_depth_extract.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "cs_main",
                            "cs_5_1", flags, 0, _depthExtractShaderBlob.GetAddressOf(), error.GetAddressOf());
    if (nullptr != error)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
        GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
    }
    FAILED_CHECK_MESSAGE(hr,L"");

    // 히스토그램 계산 셰이더
    hr = D3DCompileFromFile(L"../Shaders/cs_radix_histogram.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                            "cs_main", "cs_5_1", flags, 0, _histogramShaderBlob.GetAddressOf(), error.GetAddressOf());
    if (nullptr != error)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
        GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
    }
    FAILED_CHECK_MESSAGE(hr,L"");

    // 접두사 합 셰이더
    hr = D3DCompileFromFile(L"../Shaders/cs_prefix_sum.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "cs_main",
                            "cs_5_1", flags, 0, _prefixSumShaderBlob.GetAddressOf(), error.GetAddressOf());
    if (nullptr != error)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
        GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
    }

    FAILED_CHECK_MESSAGE(hr,L"");

    // 스캐터 셰이더
    hr = D3DCompileFromFile(L"../Shaders/cs_radix_scatter.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "cs_main",
                            "cs_5_1", flags, 0, _scatterShaderBlob.GetAddressOf(), error.GetAddressOf());
    if (nullptr != error)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
        GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
    }
    FAILED_CHECK_MESSAGE(hr,L"");

    // 재정렬 셰이더 (정렬된 인덱스로 ParticleOutput 재배열)
    hr = D3DCompileFromFile(L"../Shaders/cs_particle_reorder.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
                            "cs_main", "cs_5_1", flags, 0, _reorderShaderBlob.GetAddressOf(), error.GetAddressOf());
    if (nullptr != error)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
        GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
    }
    FAILED_CHECK_MESSAGE(hr,L"");
}
void ParticleManager::InitializeRadixSortRootSignature() 
{

       std::vector<D3D12_ROOT_PARAMETER> rootParameters;
    rootParameters.resize(8);

    // CBV (b0) - 모든 셰이더에서 사용하는 RadixSortConstants
    rootParameters[0].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].Descriptor.ShaderRegister = 0;
    rootParameters[0].Descriptor.RegisterSpace  = 0;
    rootParameters[0].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    // SRV (t0) - 입력 데이터 (ParticleOutput 또는 정렬 키)
    rootParameters[1].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParameters[1].Descriptor.ShaderRegister = 0;
    rootParameters[1].Descriptor.RegisterSpace  = 0;
    rootParameters[1].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    // SRV (t1) - 입력 값 (스캐터 단계에서 사용)
    rootParameters[2].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_SRV;
    rootParameters[2].Descriptor.ShaderRegister = 1;
    rootParameters[2].Descriptor.RegisterSpace  = 0;
    rootParameters[2].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    // UAV (u0) - 출력 키 버퍼
    rootParameters[3].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
    rootParameters[3].Descriptor.ShaderRegister = 0;
    rootParameters[3].Descriptor.RegisterSpace  = 0;
    rootParameters[3].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    // UAV (u1) - 출력 값 버퍼
    rootParameters[4].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
    rootParameters[4].Descriptor.ShaderRegister = 1;
    rootParameters[4].Descriptor.RegisterSpace  = 0;
    rootParameters[4].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    // UAV (u2) - 히스토그램 버퍼
    rootParameters[5].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
    rootParameters[5].Descriptor.ShaderRegister = 2;
    rootParameters[5].Descriptor.RegisterSpace  = 0;
    rootParameters[5].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    // UAV (u3) - 접두사 합 버퍼
    rootParameters[6].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
    rootParameters[6].Descriptor.ShaderRegister = 3;
    rootParameters[6].Descriptor.RegisterSpace  = 0;
    rootParameters[6].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    // UAV (u4) - ParticleOutput 재정렬 버퍼
    rootParameters[7].ParameterType             = D3D12_ROOT_PARAMETER_TYPE_UAV;
    rootParameters[7].Descriptor.ShaderRegister = 4;
    rootParameters[7].Descriptor.RegisterSpace  = 0;
    rootParameters[7].ShaderVisibility          = D3D12_SHADER_VISIBILITY_ALL;

    D3D12_ROOT_SIGNATURE_DESC rootSignDesc = {};
    rootSignDesc.NumParameters             = rootParameters.size();
    rootSignDesc.pParameters               = rootParameters.data();
    rootSignDesc.Flags                     = D3D12_ROOT_SIGNATURE_FLAG_NONE;
    rootSignDesc.NumStaticSamplers         = 0;
    rootSignDesc.pStaticSamplers           = nullptr;

    // 직렬화 및 루트 시그니처 생성
    ComPtr<ID3DBlob> serializedRootSig;
    ComPtr<ID3DBlob> error;
    HRESULT          hr = D3D12SerializeRootSignature(&rootSignDesc, D3D_ROOT_SIGNATURE_VERSION_1,
                                                      serializedRootSig.GetAddressOf(), error.GetAddressOf());
    FAILED_CHECK_MESSAGE(hr,L"");

    hr = UmDevice.GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
                                                   serializedRootSig->GetBufferSize(),
                                                   IID_PPV_ARGS(_radixSortRootSignature.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr,L"");
}
void ParticleManager::InitializeRadixSortPSO() 
{
    HRESULT hr = S_OK;

    // 1. 깊이 추출 PSO 생성 - 통합 루트 시그니처 사용
    D3D12_COMPUTE_PIPELINE_STATE_DESC depthExtractPsoDesc = {};
    depthExtractPsoDesc.pRootSignature                    = _radixSortRootSignature.Get(); // 수정
    depthExtractPsoDesc.CS = {_depthExtractShaderBlob->GetBufferPointer(), _depthExtractShaderBlob->GetBufferSize()};
    depthExtractPsoDesc.NodeMask = 0;
    depthExtractPsoDesc.Flags    = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr = UmDevice.GetDevice()->CreateComputePipelineState(&depthExtractPsoDesc,
                                                          IID_PPV_ARGS(_depthExtractPSO.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr,L"");

    // 2. 히스토그램 PSO 생성 - 통합 루트 시그니처 사용
    D3D12_COMPUTE_PIPELINE_STATE_DESC histogramPsoDesc = {};
    histogramPsoDesc.pRootSignature                    = _radixSortRootSignature.Get(); // 수정
    histogramPsoDesc.CS       = {_histogramShaderBlob->GetBufferPointer(), _histogramShaderBlob->GetBufferSize()};
    histogramPsoDesc.NodeMask = 0;
    histogramPsoDesc.Flags    = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr =
        UmDevice.GetDevice()->CreateComputePipelineState(&histogramPsoDesc, IID_PPV_ARGS(_histogramPSO.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr,L"");

    // 3. 접두사 합 PSO 생성 - 통합 루트 시그니처 사용
    D3D12_COMPUTE_PIPELINE_STATE_DESC prefixSumPsoDesc = {};
    prefixSumPsoDesc.pRootSignature                    = _radixSortRootSignature.Get(); // 수정
    prefixSumPsoDesc.CS       = {_prefixSumShaderBlob->GetBufferPointer(), _prefixSumShaderBlob->GetBufferSize()};
    prefixSumPsoDesc.NodeMask = 0;
    prefixSumPsoDesc.Flags    = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr =
        UmDevice.GetDevice()->CreateComputePipelineState(&prefixSumPsoDesc, IID_PPV_ARGS(_prefixSumPSO.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr,L"");

    // 4. 스캐터 PSO 생성 - 통합 루트 시그니처 사용
    D3D12_COMPUTE_PIPELINE_STATE_DESC scatterPsoDesc = {};
    scatterPsoDesc.pRootSignature                    = _radixSortRootSignature.Get(); // 수정
    scatterPsoDesc.CS       = {_scatterShaderBlob->GetBufferPointer(), _scatterShaderBlob->GetBufferSize()};
    scatterPsoDesc.NodeMask = 0;
    scatterPsoDesc.Flags    = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr = UmDevice.GetDevice()->CreateComputePipelineState(&scatterPsoDesc, IID_PPV_ARGS(_scatterPSO.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr,L"");

    // 5. 재정렬 PSO 생성 - 통합 루트 시그니처 사용
    D3D12_COMPUTE_PIPELINE_STATE_DESC reorderPsoDesc = {};
    reorderPsoDesc.pRootSignature                    = _radixSortRootSignature.Get(); // 수정
    reorderPsoDesc.CS       = {_reorderShaderBlob->GetBufferPointer(), _reorderShaderBlob->GetBufferSize()};
    reorderPsoDesc.NodeMask = 0;
    reorderPsoDesc.Flags    = D3D12_PIPELINE_STATE_FLAG_NONE;

    hr = UmDevice.GetDevice()->CreateComputePipelineState(&reorderPsoDesc, IID_PPV_ARGS(_reorderPSO.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr,L"");
}
void ParticleManager::CreateRadixSortResources() 
{
    // 정렬 키-값 버퍼 생성 (float depth + uint index)
    UINT sortBufferSize = _maxParticles * sizeof(ParticleSortData);
    CreateUAVBuffer(_sortKeysBuffer, _maxParticles * sizeof(float), sizeof(float));
    CreateUAVBuffer(_sortValuesBuffer, _maxParticles * sizeof(UINT), sizeof(UINT));
    CreateUAVBuffer(_sortKeysAltBuffer, _maxParticles * sizeof(float), sizeof(float));
    CreateUAVBuffer(_sortValuesAltBuffer, _maxParticles * sizeof(UINT), sizeof(UINT));
    CreateUAVBuffer(_sortedOutputBuffer, _maxParticles * sizeof(ParticleOutput), sizeof(ParticleOutput));
    // 히스토그램 버퍼 (각 스레드 그룹별로 RADIX_SIZE개 카운터)
    UINT numThreadGroups = (_maxParticles + THREADS_PER_GROUP - 1) / THREADS_PER_GROUP;
    UINT histogramSize   = numThreadGroups * RADIX_SIZE * sizeof(UINT);
    CreateUAVBuffer(_histogramBuffer, histogramSize, sizeof(UINT));

    // 접두사 합 버퍼
    UINT prefixSumSize = RADIX_SIZE * sizeof(UINT);
    CreateUAVBuffer(_prefixSumBuffer, prefixSumSize, sizeof(UINT));

    // 정렬 상수 버퍼
    for (UINT bit = 0; bit < 32; bit += RADIX_BITS)
    {
        ComPtr<ID3D12Resource> tempbuff;
        CreateConstantBuffer(tempbuff, sizeof(RadixSortConstants));
        _sortConstantBuffer.push_back(tempbuff);
    }
}
void ParticleManager::PerformRadixSort() 
{
        _radixSortAllocator->Reset();
        _radixSortCommandList->Reset(_radixSortAllocator.Get(), nullptr);

    // 2. 32비트 float를 위한 8번의 4비트 radix 패스 실행
    for (UINT bit = 0; bit < 32; bit += RADIX_BITS)
    {

        // 상수 버퍼 업데이트
        RadixSortConstants constants;
        constants.numParticles    = _totalCount;
        constants.currentBit      = bit;
        constants.numThreadGroups = (_totalCount + THREADS_PER_GROUP - 1) / THREADS_PER_GROUP;
        void* mappedData = nullptr;
        _sortConstantBuffer[bit/4]->Map(0, nullptr, &mappedData);
        memcpy(mappedData, &constants, sizeof(RadixSortConstants));
        _sortConstantBuffer[bit / 4]->Unmap(0, nullptr);

        // 통합 루트 시그니처 설정
        _radixSortCommandList->SetComputeRootSignature(_radixSortRootSignature.Get());

        // 공통 상수 버퍼 바인딩
        _radixSortCommandList->SetComputeRootConstantBufferView(0,
                                                                _sortConstantBuffer[bit / 4]->GetGPUVirtualAddress());

        // === 히스토그램 계산 단계 ===
        _radixSortCommandList->SetPipelineState(_histogramPSO.Get());

        // 입력 키 버퍼 바인딩 (ping-pong)
        UINT                   passIndex         = bit / RADIX_BITS;
        ComPtr<ID3D12Resource> currentKeysBuffer = (passIndex % 2 == 0) ? _sortKeysBuffer : _sortKeysAltBuffer;
        
        _radixSortCommandList->SetComputeRootShaderResourceView(1, currentKeysBuffer->GetGPUVirtualAddress());



        // 사용하지 않는 SRV 슬롯
        _radixSortCommandList->SetComputeRootShaderResourceView(2, 0);

        // 사용하지 않는 UAV 슬롯들
        _radixSortCommandList->SetComputeRootUnorderedAccessView(3, 0);
        _radixSortCommandList->SetComputeRootUnorderedAccessView(4, 0);

        // 히스토그램 출력 버퍼
        _radixSortCommandList->SetComputeRootUnorderedAccessView(5, _histogramBuffer->GetGPUVirtualAddress());

        // 사용하지 않는 UAV 슬롯들
        _radixSortCommandList->SetComputeRootUnorderedAccessView(6, 0);
        _radixSortCommandList->SetComputeRootUnorderedAccessView(7, 0);


        // 메모리 배리어
        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::UAV(_histogramBuffer.Get());
        _radixSortCommandList->ResourceBarrier(1, &barrier);

        _radixSortCommandList->Dispatch(constants.numThreadGroups, 1, 1);





        // === 접두사 합 계산 단계 ===
        _radixSortCommandList->SetPipelineState(_prefixSumPSO.Get());

        // 사용하지 않는 SRV 슬롯들
        _radixSortCommandList->SetComputeRootShaderResourceView(1, 0);
        _radixSortCommandList->SetComputeRootShaderResourceView(2, 0);

        // 사용하지 않는 UAV 슬롯들
        _radixSortCommandList->SetComputeRootUnorderedAccessView(3, 0);
        _radixSortCommandList->SetComputeRootUnorderedAccessView(4, 0);

        // 히스토그램 버퍼 (읽기/쓰기)
        _radixSortCommandList->SetComputeRootUnorderedAccessView(5, _histogramBuffer->GetGPUVirtualAddress());

        // 접두사 합 출력 버퍼
        _radixSortCommandList->SetComputeRootUnorderedAccessView(6, _prefixSumBuffer->GetGPUVirtualAddress());

        // 사용하지 않는 UAV 슬롯
        _radixSortCommandList->SetComputeRootUnorderedAccessView(7, 0);

        // 메모리 배리어
        barrier = CD3DX12_RESOURCE_BARRIER::UAV(_prefixSumBuffer.Get());
        _radixSortCommandList->ResourceBarrier(1, &barrier);

        barrier = CD3DX12_RESOURCE_BARRIER::UAV(_histogramBuffer.Get());
        _radixSortCommandList->ResourceBarrier(1, &barrier);

        _radixSortCommandList->Dispatch(1, 1, 1);








        // === 스캐터 (재배열) 단계 ===
        _radixSortCommandList->SetPipelineState(_scatterPSO.Get());

        // 입력 키 버퍼
        _radixSortCommandList->SetComputeRootShaderResourceView(1, currentKeysBuffer->GetGPUVirtualAddress());

        // 입력 값 버퍼 (ping-pong)
        ComPtr<ID3D12Resource> currentValuesBuffer = (bit % 8 == 0) ? _sortValuesBuffer : _sortValuesAltBuffer;
        _radixSortCommandList->SetComputeRootShaderResourceView(2, currentValuesBuffer->GetGPUVirtualAddress());

        // 출력 키 버퍼 (ping-pong)
        ComPtr<ID3D12Resource> outputKeysBuffer = (passIndex % 2 == 0) ? _sortKeysAltBuffer : _sortKeysBuffer;
        _radixSortCommandList->SetComputeRootUnorderedAccessView(3, outputKeysBuffer->GetGPUVirtualAddress());

        // 출력 값 버퍼 (ping-pong)
        ComPtr<ID3D12Resource> outputValuesBuffer = (bit % 8 == 0) ? _sortValuesAltBuffer : _sortValuesBuffer;
        _radixSortCommandList->SetComputeRootUnorderedAccessView(4, outputValuesBuffer->GetGPUVirtualAddress());

        // 히스토그램 버퍼 (오프셋으로 사용)
        _radixSortCommandList->SetComputeRootUnorderedAccessView(5, _histogramBuffer->GetGPUVirtualAddress());

        // 접두사 합 버퍼
        _radixSortCommandList->SetComputeRootUnorderedAccessView(6, _prefixSumBuffer->GetGPUVirtualAddress());

        // 사용하지 않는 UAV 슬롯
        _radixSortCommandList->SetComputeRootUnorderedAccessView(7, 0);

        barrier = CD3DX12_RESOURCE_BARRIER::UAV(currentValuesBuffer.Get());
        _radixSortCommandList->ResourceBarrier(1, &barrier);

        barrier = CD3DX12_RESOURCE_BARRIER::UAV(outputKeysBuffer.Get());
        _radixSortCommandList->ResourceBarrier(1, &barrier);

        barrier = CD3DX12_RESOURCE_BARRIER::UAV(outputValuesBuffer.Get());
        _radixSortCommandList->ResourceBarrier(1, &barrier);

        barrier = CD3DX12_RESOURCE_BARRIER::UAV(_histogramBuffer.Get());
        _radixSortCommandList->ResourceBarrier(1, &barrier);

        barrier = CD3DX12_RESOURCE_BARRIER::UAV(_prefixSumBuffer.Get());
        _radixSortCommandList->ResourceBarrier(1, &barrier);

        _radixSortCommandList->Dispatch(constants.numThreadGroups, 1, 1);

    }
        _radixSortCommandList->Close();
    UmDevice.RegisterCommand(_radixSortCommandList.Get(), PARTICLE_COMPUTE_LIST);

    // 3. 정렬된 인덱스로 ParticleOutput 재정렬

}
void ParticleManager::ExtractDepthKeys() 
{
    _depthExtractAllocator->Reset();
    _depthExtractCommandList->Reset(_depthExtractAllocator.Get(), _depthExtractPSO.Get());

    // 통합 루트 시그니처 설정
    _depthExtractCommandList->SetComputeRootSignature(_radixSortRootSignature.Get());
    // 파라미터 인덱스에 맞춰 리소스 바인딩
    // [0] CBV (b0) - 상수 버퍼
    _depthExtractCommandList->SetComputeRootConstantBufferView(0, _sortConstantBuffer[0]->GetGPUVirtualAddress());

    // [1] SRV (t0) - ParticleOutput 입력 버퍼
    _depthExtractCommandList->SetComputeRootShaderResourceView(1, _particleOutputBuffer->GetGPUVirtualAddress());

    // [2] SRV (t1) - 사용하지 않으므로 NULL 바인딩
    _depthExtractCommandList->SetComputeRootShaderResourceView(2, 0); // NULL 바인딩

    // [3] UAV (u0) - 정렬 키 출력 버퍼
    _depthExtractCommandList->SetComputeRootUnorderedAccessView(3, _sortKeysBuffer->GetGPUVirtualAddress());

    // [4] UAV (u1) - 정렬 값 출력 버퍼
    _depthExtractCommandList->SetComputeRootUnorderedAccessView(4, _sortValuesBuffer->GetGPUVirtualAddress());

    // [5] UAV (u2) - 사용하지 않으므로 NULL 바인딩
    _depthExtractCommandList->SetComputeRootUnorderedAccessView(5, 0); // NULL 바인딩

    // [6] UAV (u3) - 사용하지 않으므로 NULL 바인딩
    _depthExtractCommandList->SetComputeRootUnorderedAccessView(6, 0); // NULL 바인딩

    // [7] UAV (u4) - 사용하지 않으므로 NULL 바인딩
    _depthExtractCommandList->SetComputeRootUnorderedAccessView(7, 0); // NULL 바인딩

    UINT numThreadGroups = (_totalCount + THREADS_PER_GROUP - 1) / THREADS_PER_GROUP;
    _depthExtractCommandList->Dispatch(numThreadGroups, 1, 1);
    _depthExtractCommandList->Close();
    UmDevice.RegisterCommand(_depthExtractCommandList.Get(), PARTICLE_COMPUTE_LIST);
}
void ParticleManager::ReorderParticleOutput() 
{
    _reorderAllocator->Reset();
    _reorderCommandList->Reset(_reorderAllocator.Get(), _reorderPSO.Get());
    // 정렬된 인덱스를 사용하여 ParticleOutput 재배열
    _reorderCommandList->SetComputeRootSignature(_radixSortRootSignature.Get());
    _reorderCommandList->SetComputeRootConstantBufferView(0, _sortConstantBuffer[7]->GetGPUVirtualAddress());
    _reorderCommandList->SetComputeRootShaderResourceView(1, _sortValuesBuffer->GetGPUVirtualAddress());
    _reorderCommandList->SetComputeRootShaderResourceView(2, _particleOutputBuffer->GetGPUVirtualAddress());
    _reorderCommandList->SetComputeRootUnorderedAccessView(7, _sortedOutputBuffer->GetGPUVirtualAddress());

    //auto barrior = D3D12_RESOURCE_BARRIER::UAV()

    UINT numThreadGroups = (_totalCount + THREADS_PER_GROUP - 1) / THREADS_PER_GROUP;
    _reorderCommandList->Dispatch(numThreadGroups, 1, 1);
    _reorderCommandList->Close();
    UmDevice.RegisterCommand(_reorderCommandList.Get(), PARTICLE_COMPUTE_LIST);
}

