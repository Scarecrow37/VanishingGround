#include "pch.h"
#include "DownSamplePass.h"
#include "Quad.h"
#include "RenderScene.h"
#include "RenderTarget.h"
#include "ShaderBuilder.h"

DownSamplePass::DownSamplePass() {}

DownSamplePass::~DownSamplePass() {}

void DownSamplePass::Initialize(const D3D12_VIEWPORT& viewPort, const D3D12_RECT& sissorRect)
{
    /*__super::Initialize(viewPort, sissorRect);

    _shader = std::make_shared<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shaders/cs_down_sample.hlsl", ShaderBuilder::Type::CS);
    _shader->EndBuild(ShaderBuilder::BindType::DIRECT);

    ID3D12Device*                     device = UmDevice.GetDevice();
    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{.pRootSignature = _shader->GetRootSignature(),
                                              .CS             = _shader->GetShaderByteCode(ShaderBuilder::Type::CS)};

    HRESULT hr = S_OK;
    hr         = device->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(&_pipelineState));
    FAILED_CHECK_MESSAGE(hr, L"DownSamplePass::Initialize device->CreateGraphicsPipelineState Failed");

    D3D12_RESOURCE_DESC textureDesc{.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
                                    .Width     = viewPort.Width,
                                    .Height    = viewPort.Height,
                                    .Format    = DXGI_FORMAT_R32G32B32A32_FLOAT,
                                    .Flags     = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS};

    auto heapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    device->CreateCommittedResource(&heapProperty, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&_resource));

    D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{.Format        = DXGI_FORMAT_R32G32B32A32_FLOAT,
                                             .ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D};
    
    UmViewManager.AddDescriptorHeap(ViewManager::Type::SHADER_RESOURCE, _handle);

    device->CreateUnorderedAccessView(_resource.Get(), nullptr, &uavDesc, _handle.CPU);*/
}

void DownSamplePass::Begin(ID3D12GraphicsCommandList* commandList)
{
    /*commandList->OMSetRenderTargets(1, &_ownerScene->_renderTargets[0]->GetRTVHandle(), FALSE, nullptr);
    commandList->RSSetViewports(1, &_viewPort);
    commandList->RSSetScissorRects(1, &_sissorRect);*/
}

void DownSamplePass::End(ID3D12GraphicsCommandList* commandList)
{
    ID3D12Resource* rt = _ownerScene->_renderTargets[0]->GetResource();
    auto            br = CD3DX12_RESOURCE_BARRIER::Transition(rt, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &br);
}

void DownSamplePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    const auto&           mode     = UmDevice.GetMode();
    auto                  resource = UmViewManager.GetShaderResourceHeap();
    ID3D12DescriptorHeap* hps[]    = { resource, };
    
    PostProcessData postProcessData{ .TexelSize = {1.f / mode.Width, 1.f / mode.Height}, };

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetComputeRootSignature(_shader->GetRootSignature());
    commandList->SetDescriptorHeaps(_countof(hps), hps);
    
    commandList->SetComputeRoot32BitConstants(_shader->GetRootParameterIndex("bit32_5_postProcessData"), 5, &postProcessData, 0);
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("inputTexture"), _ownerScene->_renderTargets[0]->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("outputTexture"), _handle.GPU);
    
    UINT width = std::max(1u, (UINT)_viewPort.Width / 8);
    UINT height = std::max(1u, (UINT)_viewPort.Height / 8);
    commandList->Dispatch(width, height, 1);
}