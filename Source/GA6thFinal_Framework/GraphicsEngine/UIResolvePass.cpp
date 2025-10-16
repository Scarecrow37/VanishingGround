#include "pch.h"
#include "UIResolvePass.h"

UIResolvePass::UIResolvePass() = default;

UIResolvePass::~UIResolvePass() = default;

void UIResolvePass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    UIPassBase_OIT::Initialize(ownerScene, ownerTechnique, commandList);

    _outputBuffer = MakeSharedResource<UnorderedAccessView>();
    {
        const auto& resolution = Global::device->GetResolution();
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_FLOAT, resolution.cx, resolution.cy, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        _outputBuffer->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP, false);
    }

    ComputePipelineStateStream pss;
    _fxResolve.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}

void UIResolvePass::Begin(ID3D12GraphicsCommandList* commandList)
{
    _outputBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

    _outputBuffer->ClearUnorderedAccessView(commandList, Vector4(0, 0, 0, 0));
}

void UIResolvePass::Draw(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetComputeRootSignature(_fxResolve.GetRootSignature());

    commandList->SetComputeRootDescriptorTable(_fxResolve.GetRootParameterIndex("screenTexture"), _finalRenderTarget->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_fxResolve.GetRootParameterIndex("OITHead"), _headBuffer->GetUAVHandle());
    commandList->SetComputeRootUnorderedAccessView(_fxResolve.GetRootParameterIndex("OITNodes"), _nodesBuffer->GetGPUVirtualAddress());
    commandList->SetComputeRootDescriptorTable(_fxResolve.GetRootParameterIndex("Output"), _outputBuffer->GetUAVHandle());    

    const auto& resolution = Global::device->GetResolution();
    UINT        dispatchX  = (resolution.cx + 15) / 16;
    UINT        dispatchY  = (resolution.cy + 15) / 16;
    commandList->Dispatch(dispatchX, dispatchY, 1);

    _outputBuffer->ResourceBarrier(commandList);
}

void UIResolvePass::End(ID3D12GraphicsCommandList* commandList)
{
    _outputBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_SOURCE);
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_COPY_DEST);

    commandList->CopyResource(_finalRenderTarget->GetResource(), _outputBuffer->GetResource());

    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}