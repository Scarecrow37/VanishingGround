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
    D3D12_COMMAND_QUEUE_DESC desc
    {
        .Type     = D3D12_COMMAND_LIST_TYPE_COMPUTE,
        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE,
        .NodeMask = 0,
     };
     FAILED_CHECK_BREAK(_device->CreateCommandQueue(&desc, IID_PPV_ARGS(_computeQueue.GetAddressOf())));
     FAILED_CHECK_BREAK(_device->CreateCommandAllocator(desc.Type, IID_PPV_ARGS(_computeAllocator.GetAddressOf())));
     FAILED_CHECK_BREAK(_device->CreateCommandList(desc.NodeMask, desc.Type, _computeAllocator.Get(), nullptr,
                                                   IID_PPV_ARGS(_computeCmdList.GetAddressOf())));
     _computeCmdList->Close();

         // 펜스 생성 (초기값 0)
     _device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&_computeFence));

     // 이벤트 핸들 생성 (동기화용)
     _fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);


 }

 void ParticleManager::Update(const float deltaTime) 
{



}
