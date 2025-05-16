#include "pch.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"
#include "ParticleManager.h"

 ParticleManager::ParticleManager()
{
     _currentBufferIndex = 0;
     _particleStride     = sizeof(Particle);
  
 }

void ParticleManager::Initialize(UINT maxParticles)
 {
    _maxParticles = maxParticles;
     InitializeComputeCommandObject();
     InitializeRenderCommandList();
     InitializeComputeSyncObject();
     InitializeParticleComputeShader();
     InitializeParticleComputeRootSignature();
     InitializeParticleComputePSO();
     //InitializeSortingComputeShader();
     //InitializeSortingComputeRootSignature();
     //InitializeSortingComputePSO();
     
 }

 void ParticleManager::Update(const float deltaTime) 
{

     //컴퓨트 

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
void ParticleManager::InitializeComputeSyncObject() 
{
    // 펜스 생성 (초기값 0)
    UmDevice.GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_computeFence));

    // 이벤트 핸들 생성 (동기화용)
    _fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
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

}

