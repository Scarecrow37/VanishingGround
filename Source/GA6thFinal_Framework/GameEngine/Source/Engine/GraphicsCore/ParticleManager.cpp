#include "pch.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"
#include "ParticleManager.h"

void ParticleManager::SetCamera(std::string_view viewName) 
{
    _camera = UmRenderer.GetCamera(viewName);
}

ParticleManager::ParticleManager()
{

  
 }

void ParticleManager::Initialize(UINT maxParticles)
 {
    _currentBufferIndex = 0;
    _particleStride     = sizeof(Particle);
    _maxParticles = maxParticles;
     InitializeRenderCommandList();
     InitializeComputeCommandObject();
     InitializeParticleComputeShader();
     InitializeParticleComputeRootSignature();
     InitializeParticleComputePSO();
     //InitializeSortingComputeShader();
     //InitializeSortingComputeRootSignature();
     //InitializeSortingComputePSO();
     InitializeDescriptorHeap();
     
 }

void ParticleManager::RegisterEffect() 
{

}

 void ParticleManager::Update(const float deltaTime)
{
    for (auto effect : _pariticleEffects)
    {
        effect->Update(deltaTime);
    }
    _totalParticles.clear();
    _emitterMatrix.clear();
    UINT emitterIndex = 0;
    for (auto effect : _pariticleEffects)
    {
        if (true == effect->GetActiveFlag())
        {
            for (auto emitter : effect->GetEmitterList())
            {
                if (true == emitter->GetActiveFlag())
                {
                    _emitterMatrix.push_back({emitter->GetWorldMatrix()});
                    for (int i = 0; i < emitter->GetActiveParticleCount(); i++)
                    {
                        auto particle = *(emitter->GetParticlePool()[i]);
                        particle.SetEmitterIndex(emitterIndex);
                        _totalParticles.push_back(*(emitter->GetParticlePool()[i]));
                    }
                    emitterIndex++;
                }
            }
        }
    }

    // dispatch compute shader
    {
        DispatchParticleCompute(deltaTime);
        UmDevice.RegisterCommand(_computeCommandList.Get(), PARTICLE_COMPUTE_LIST);
    }
    // update particle lifecycle
    for (auto effect : _pariticleEffects)
    {
        if (true == effect->GetActiveFlag())
        {
            effect->UpdateParticleLifeCycle(deltaTime);
        }
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
    

    FAILED_CHECK_BREAK(UmDevice.GetDevice()->CreateCommandQueue(&desc, IID_PPV_ARGS(_computeQueue.GetAddressOf())));


    FAILED_CHECK_BREAK(UmDevice.GetDevice()->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(_computeAllocator.GetAddressOf())));
    FAILED_CHECK_BREAK(UmDevice.GetDevice()->CreateCommandList(desc.NodeMask, desc.Type, _computeAllocator.Get(), nullptr,
                                                  IID_PPV_ARGS(_computeCommandList.GetAddressOf())));


    _computeCommandList->Close();
}
void ParticleManager::InitializeRenderCommandList()
{
    FAILED_CHECK_BREAK(UmDevice.GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                       IID_PPV_ARGS(_particleCommandAllocator.GetAddressOf())));
    FAILED_CHECK_BREAK(UmDevice.GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _particleCommandAllocator.Get(),
                                                  nullptr, IID_PPV_ARGS(_particleCommandList.GetAddressOf())));
    _particleCommandList->Close();
}

void ParticleManager::InitializeParticleComputeShader()
{
     
    
    HRESULT          hr = S_OK;
    ComPtr<ID3DBlob> error;
    //non-axial billboard sprite particle compute shader
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
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, _computeSpriteShaderBlob.GetAddressOf(),
                                error.GetAddressOf());

        if (nullptr != error)
        {
            std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
            ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }

        FAILED_CHECK_BREAK(hr);
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
            ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }

        FAILED_CHECK_BREAK(hr);
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
            ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }

        FAILED_CHECK_BREAK(hr);
    }

}
void ParticleManager::InitializeParticleComputeRootSignature() 
{
    //initialize sprite root signature;
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
            ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }


        FAILED_CHECK_BREAK(hr);

        ComPtr<ID3D12RootSignature> rootSignature;
        hr = UmDevice.GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
                                          IID_PPV_ARGS(_computeSpriteRootSignature.GetAddressOf()));
        FAILED_CHECK_BREAK(hr);
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
            ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }


        FAILED_CHECK_BREAK(hr);

        ComPtr<ID3D12RootSignature> rootSignature;
        hr = UmDevice.GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
                                          IID_PPV_ARGS(_computeMeshRootSignature.GetAddressOf()));
        FAILED_CHECK_BREAK(hr);
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
        FAILED_CHECK_BREAK(hr);

    }
    // initialize axial sprite pipeline state object
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC computePSODesc = {};
        ZeroMemory(&computePSODesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
        computePSODesc.CS = {_computeAxialSpriteShaderBlob->GetBufferPointer(),
                             _computeAxialSpriteShaderBlob->GetBufferSize()};
        computePSODesc.pRootSignature = _computeSpriteRootSignature.Get();

        HRESULT hr;
        hr = UmDevice.GetDevice()->CreateComputePipelineState(&computePSODesc,
                                                              IID_PPV_ARGS(_computeAxialSpritePSO.GetAddressOf()));
        FAILED_CHECK_BREAK(hr);
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
        FAILED_CHECK_BREAK(hr);
    }



}

void ParticleManager::InitializeSortingComputeShader()
{

    HRESULT          hr = S_OK;
    ComPtr<ID3DBlob> error;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES |
                 D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    hr = D3DCompileFromFile(L"../Shaders/cs_sort_particle.hlsl", // HLSL 파일 경로
                            nullptr, nullptr,
                            "CSMain", // 셰이더 진입점
                            "cs_5_1", // 셰이더 모델
                            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, _sortingShaderBlob.GetAddressOf(),
                            nullptr);

    if (nullptr != error)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
        ASSERT(SUCCEEDED(hr), errorMessage.c_str());
    }

    FAILED_CHECK_BREAK(hr);
}
void ParticleManager::InitializeSortingComputeRootSignature() 
{
    // initialize sorting root signature;
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
            ASSERT(SUCCEEDED(hr), errorMessage.c_str());
        }


        FAILED_CHECK_BREAK(hr);

        ComPtr<ID3D12RootSignature> rootSignature;
        hr = UmDevice.GetDevice()->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
                                          IID_PPV_ARGS(_sortingRootSignature.GetAddressOf()));
        FAILED_CHECK_BREAK(hr);
    }
}
void ParticleManager::InitializeSortingComputePSO() 
{
    // initialize sorting pipeline state object
    {
        D3D12_COMPUTE_PIPELINE_STATE_DESC computePSODesc = {};
        ZeroMemory(&computePSODesc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
        computePSODesc.CS = {_sortingShaderBlob->GetBufferPointer(), _sortingShaderBlob->GetBufferSize()};
        computePSODesc.pRootSignature = _sortingRootSignature.Get();

        HRESULT hr;
        hr = UmDevice.GetDevice()->CreateComputePipelineState(&computePSODesc, IID_PPV_ARGS(_sortingPSO.GetAddressOf()));
        FAILED_CHECK_BREAK(hr);
    }
}

void ParticleManager::InitializeDescriptorHeap() 
{
    // 디스크립터 힙 생성 (CBV 1개 + SRV 2개 + UAV 1개 = 총 4개)
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
    heapDesc.NumDescriptors = 4;
    heapDesc.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    FAILED_CHECK_BREAK(UmDevice.GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(_cbvSrvUavHeap.GetAddressOf())));

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

    // 4. MVP 상수 버퍼 (CBV - b0)
    UINT mvpConstantSize = BYTEALIGN(sizeof(MVPConstants), 256); // 256바이트 정렬
    CreateConstantBuffer(_mvpConstantBuffer, _mvpUploadBuffer, mvpConstantSize);

    // 디스크립터 생성
    CreateDescriptors();
}

void ParticleManager::CreateStructuredBuffer(ComPtr<ID3D12Resource>& resource, ComPtr<ID3D12Resource>& uploadResource,
                                             UINT bufferSize, UINT stride)
{
    // 기본 버퍼 생성 (GPU 전용)
    D3D12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    auto                defaultProperty   = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    FAILED_CHECK_BREAK(UmDevice.GetDevice()->CreateCommittedResource(&defaultProperty, D3D12_HEAP_FLAG_NONE,
                                                                     &bufferDesc,
        D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resource)));

    // 업로드 버퍼 생성 (CPU->GPU 전송용)
    auto uploadProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    FAILED_CHECK_BREAK(UmDevice.GetDevice()->CreateCommittedResource(&uploadProperty, D3D12_HEAP_FLAG_NONE, &bufferDesc,
                                                                     D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                                     IID_PPV_ARGS(&uploadResource)));


}

void ParticleManager::CreateUAVBuffer(ComPtr<ID3D12Resource>& resource, UINT bufferSize, UINT stride) 
{
    // UAV 버퍼 생성 (GPU 읽기/쓰기)
    D3D12_RESOURCE_DESC bufferDesc =
        CD3DX12_RESOURCE_DESC::Buffer(bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
    auto defaultProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    FAILED_CHECK_BREAK(UmDevice.GetDevice()->CreateCommittedResource(&defaultProperty, D3D12_HEAP_FLAG_NONE,
                                                                     &bufferDesc,
        D3D12_RESOURCE_STATE_UNORDERED_ACCESS, nullptr, IID_PPV_ARGS(&resource)));
}

void ParticleManager::CreateConstantBuffer(ComPtr<ID3D12Resource>& resource, ComPtr<ID3D12Resource>& uploadResource,
                                           UINT bufferSize)
{
    // 상수 버퍼는 항상 256바이트 정렬되어야 함
    D3D12_RESOURCE_DESC bufferDesc     = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);
    auto uploadProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    FAILED_CHECK_BREAK(UmDevice.GetDevice()->CreateCommittedResource(
        &uploadProperty, D3D12_HEAP_FLAG_NONE,
        &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
        IID_PPV_ARGS(&resource)));

}

void ParticleManager::CreateDescriptors() 
{
    // 디스크립터 핸들 가져오기
    CD3DX12_CPU_DESCRIPTOR_HANDLE handle(_cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart());

    // 1. MVP 상수 버퍼 뷰 (b0)
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = _mvpConstantBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes    = BYTEALIGN(sizeof(MVPConstants), 256);
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
    if (_totalParticles.empty())
        return;
        
      // 1. 리소스 업데이트
    UpdateParticleResources(deltaTime);

    // 2. 컴퓨트 커맨드 리스트 리셋
    _computeAllocator->Reset();
    _computeCommandList->Reset(_computeAllocator.Get(), _computeSpritePSO.Get());


    //upload buffer -> default buf





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

    // 7. 커맨드 리스트 종료 및 실행
    _computeCommandList->Close();



}

void ParticleManager::UpdateParticleResources(float deltaTime)
{
    // 1. 파티클 입력 버퍼 업데이트
    void* mappedData = nullptr;
    _particleInputUploadBuffer->Map(0, nullptr, &mappedData);
    memcpy(mappedData, _totalParticles.data(), _totalParticles.size() * sizeof(Particle));
    _particleInputUploadBuffer->Unmap(0, nullptr);


    _emitterInfoUploadBuffer->Map(0, nullptr, &mappedData);
    memcpy(mappedData, _emitterMatrix.data(), _emitterMatrix.size() * sizeof(EmitterInfo));
    _emitterInfoUploadBuffer->Unmap(0, nullptr);

    // 3. MVP 상수 버퍼 업데이트
    MVPConstants mvpConstants;
    mvpConstants.ViewMatrix    = _camera->GetViewMatrix();
    mvpConstants.ViewInvMatrix = mvpConstants.ViewMatrix.Invert();
    mvpConstants.ProjMatrix    = _camera->GetProjectionMatrix();

    mvpConstants.CameraPos =
        Vector4(_camera->GetWorldMatrix()._41, _camera->GetWorldMatrix()._42, _camera->GetWorldMatrix()._43,1);
    mvpConstants.deltaTime     = deltaTime;

    _mvpConstantBuffer->Map(0, nullptr, &mappedData);
    memcpy(mappedData, &mvpConstants, sizeof(MVPConstants));
    _mvpConstantBuffer->Unmap(0, nullptr);
}

