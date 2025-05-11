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

void ParticleManager::InitializeComputeShader() 
{
     
    
    HRESULT          hr = S_OK;
    ComPtr<ID3DBlob> error;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES |
                 D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;

#ifdef _DEBUG
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    hr = D3DCompileFromFile(L"ComputeShader.hlsl", // HLSL 파일 경로
                            nullptr, nullptr,
                            "CSMain", // 셰이더 진입점
                            "cs_5_1", // 셰이더 모델
                            D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, _computeShaderBlob.GetAddressOf(),
                            nullptr);
    
    if (nullptr != error)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(error->GetBufferPointer());
        ASSERT(SUCCEEDED(hr), errorMessage.c_str());
    }

    FAILED_CHECK_BREAK(hr);
}

void ParticleManager::InitializeComputeRootSignature() 
{

}
 