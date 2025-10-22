#include "pch.h"
#include "PipelineStateManager.h"

ComPtr<ID3D12PipelineState> PipelineStateManager::GetPipelineState(const PipelineStateStream& stream)
{
    auto iter = _pipelineStates.find(stream);
    if (iter != _pipelineStates.end())
    {
        return iter->second;
    }

    CreatePipelineState(stream);
   
    return _pipelineStates[stream];
}

ComPtr<ID3D12PipelineState> PipelineStateManager::GetPipelineState(const ComputePipelineStateStream& stream)
{
    auto iter = _computePipelineStates.find(stream);
    if (iter != _computePipelineStates.end())
    {
        return iter->second;
    }
    ID3D12Device*                     device = Global::device->GetDevice();
    HRESULT                           hr     = S_OK;
    D3D12_COMPUTE_PIPELINE_STATE_DESC pso{
        .pRootSignature = stream.RootSignature,
        .CS             = stream.CS,
    };
    hr = device->CreateComputePipelineState(&pso, IID_PPV_ARGS(&_computePipelineStates[stream]));
    FAILED_CHECK_MESSAGE(hr, L"PipelineStateManager::GetPipelineState device->CreateComputePipelineState Failed");
    return _computePipelineStates[stream];
}

void PipelineStateManager::CreatePipelineState(const PipelineStateStream& stream)
{
    ID3D12Device*  device  = Global::device->GetDevice();
    ID3D12Device2* device2 = nullptr;
    HRESULT        hr      = device->QueryInterface(IID_PPV_ARGS(&device2));

    if (FAILED(hr))
    {
        FAILED_CHECK_MESSAGE(hr, L"PipelineStateManager::CreatePipelineState device->QueryInterface Failed");
        return;
    }

    D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = {};
    streamDesc.SizeInBytes                      = sizeof(stream);
    streamDesc.pPipelineStateSubobjectStream    = (void*)&stream;
    hr = device2->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&_pipelineStates[stream]));
    FAILED_CHECK_MESSAGE(hr, L"PipelineStateManager::CreatePipelineState device2->CreatePipelineState Failed");
    device2->Release();
}

void PipelineStateManager::CreatePipelineState(const ComputePipelineStateStream& stream)
{
    ID3D12Device*  device  = Global::device->GetDevice();
    ID3D12Device2* device2 = nullptr;
    HRESULT        hr      = device->QueryInterface(IID_PPV_ARGS(&device2));

    if (FAILED(hr))
    {
        FAILED_CHECK_MESSAGE(hr, L"PipelineStateManager::CreatePipelineState device->QueryInterface Failed");
        return;
    }

    D3D12_PIPELINE_STATE_STREAM_DESC streamDesc = {};
    streamDesc.SizeInBytes                      = sizeof(stream);
    streamDesc.pPipelineStateSubobjectStream    = (void*)&stream;
    hr = device2->CreatePipelineState(&streamDesc, IID_PPV_ARGS(&_computePipelineStates[stream]));
    FAILED_CHECK_MESSAGE(hr, L"PipelineStateManager::CreatePipelineState device2->CreatePipelineState Failed");
    device2->Release();
}