#include "pch.h"
#include "LightMapBaker.h"

LightMapBaker::LightMapBaker() {}

LightMapBaker::~LightMapBaker() {}

void LightMapBaker::Initialize(RenderScene* ownerScene)
{
    /*_ownerScene = ownerScene;

    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/cs_light_map_baking.hlsl", ShaderBuilder::Type::CS);
    _shader->EndBuild();

    ID3D12Device*                     device = Global::device->GetDevice();
    D3D12_COMPUTE_PIPELINE_STATE_DESC psodesc{.pRootSignature = _shader->GetRootSignature(),
                                              .CS             = _shader->GetShaderByteCode(ShaderBuilder::Type::CS)};

    HRESULT hr = S_OK;
    hr         = device->CreateComputePipelineState(&psodesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"LightMapBaker::Initialize device->CreateComputePipelineState Failed");

    _bakeThread = std::thread(&LightMapBaker::BakeLightMap, this);
    _bakeThread.detach();*/
}

void LightMapBaker::StartBaking()
{    
    _isBaking = true;
    _conditionVariable.notify_one();
}

void LightMapBaker::BakeLightMap()
{    
    while (true)
    {
        std::unique_lock<std::mutex> lock(_mutex);
        _conditionVariable.wait(lock, [this] { return _isBaking; });
        
        while (true)
        {
            auto commandList = Global::device->GetComputeCommandList();

            commandList->SetPipelineState(_pipelineState.Get());
            commandList->SetComputeRootSignature(_shader->GetRootSignature());

            //auto commandQueue = Global::device->GetComputeCommandQueue();
            //commandQueue->Signal()
        }
    }
}