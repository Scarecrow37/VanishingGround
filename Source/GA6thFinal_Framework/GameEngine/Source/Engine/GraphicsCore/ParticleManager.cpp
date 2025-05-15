#include "pch.h"
#include "Particle.h"
#include "ParticleEmitter.h"
#include "ParticleEffect.h"
#include "ParticleManager.h"

 ParticleManager::ParticleManager(ID3D12Device* device, UINT maxParticles) : _device(device), _maxParticles(maxParticles)
{
     _currentBufferIndex = 0;
     _particleStride     = sizeof(Particle);
     _maxParticles       = 0;
 }

void ParticleManager::Initialize()
 {

     InitializeComputeCommandObject();
     InitializeRenderCommandList();
     InitializeComputeSyncObject();


     
     
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
    FAILED_CHECK_BREAK(_device->CreateCommandQueue(&desc, IID_PPV_ARGS(_computeQueue.GetAddressOf())));


    FAILED_CHECK_BREAK(_device->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(_computeAllocator.GetAddressOf())));
    FAILED_CHECK_BREAK(_device->CreateCommandList(desc.NodeMask, desc.Type, _computeAllocator.Get(), nullptr,
                                                  IID_PPV_ARGS(_computeCommandList.GetAddressOf())));
    _computeCommandList->Close();
}

void ParticleManager::InitializeRenderCommandList()
{
    FAILED_CHECK_BREAK(_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                       IID_PPV_ARGS(_particleCommandAllocator.GetAddressOf())));
    FAILED_CHECK_BREAK(_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _particleCommandAllocator.Get(),
                                                  nullptr, IID_PPV_ARGS(_particleCommandList.GetAddressOf())));
    _particleCommandList->Close();
}

void ParticleManager::InitializeComputeSyncObject() 
{
    // 펜스 생성 (초기값 0)
    _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_computeFence));

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
                                nullptr, nullptr,
                                "cs_main", // 셰이더 진입점
                                "cs_5_1",  // 셰이더 모델
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, _computeSpriteShaderBlob.GetAddressOf(),
                                nullptr);

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
                                nullptr, nullptr,
                                "cs_main", // 셰이더 진입점
                                "cs_5_1",  // 셰이더 모델
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
                                _computeAxialSpriteShaderBlob.GetAddressOf(), nullptr);

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
                                nullptr, nullptr,
                                "cs_main", // 셰이더 진입점
                                "cs_5_1",  // 셰이더 모델
                                D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
                                _computeMeshShaderBlob.GetAddressOf(), nullptr);

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
    CD3DX12_ROOT_PARAMETER1 rootParameters[4];

    // 1. CBV (b0)
    rootParameters[0].InitAsConstantBufferView(0);

    // 2. SRV (t0)
    rootParameters[1].InitAsShaderResourceView(0);

    // 3. SRV (t1)
    rootParameters[2].InitAsShaderResourceView(1);

    // 4. UAV (u0)
    rootParameters[3].InitAsUnorderedAccessView(0);
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, // No static samplers
                               D3D12_ROOT_SIGNATURE_FLAG_NONE);

    ComPtr<ID3DBlob> serializedRootSig;
    ComPtr<ID3DBlob> errorBlob;

   hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(),
                                         errorBlob.GetAddressOf());

    ComPtr<ID3D12RootSignature> rootSignature;
    hr = device->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(),
                                IID_PPV_ARGS(&rootSignature));
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


