#include "pch.h"
#include "GITemporalPass.h"
#include "SSGITechnique.h"

void GITemporalPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
    InitShaderAnsPSO();
    _ssgiTech = dynamic_cast<SSGITechnique*>(ownerTechnique);
}

void GITemporalPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetComputeRootSignature(_fx.GetRootSignature());
}

void GITemporalPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    D3D12_GPU_VIRTUAL_ADDRESS giData        = _ssgiTech->GetConstantBufferView()->GetGPUVirtualAddress();
    bool                      currentIndex  = _ssgiTech->_currIndex;
    bool                      preIndex      = !currentIndex;
    auto                      currGITex     = _ssgiTech->_GIHalf2D[currentIndex];
    auto                      prevGITex     = _ssgiTech->_GIHalf2D[preIndex];
    auto                      temporalGITex = _ssgiTech->_GITemporalHalf;
    auto                      motionVec     = _ssgiTech->_motionVectorTex2D;

    const SIZE& res     = Global::device->GetResolution();
    SIZE        halfRes = SIZE(res.cx >> 1, res.cy >> 1);

    commandList->SetComputeRootConstantBufferView(_fx.GetRootParameterIndex("ssgiData"), giData);
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("prevHalf"), prevGITex->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("currHalf"), currGITex->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("temporalHalf"), temporalGITex->GetUAVHandle());
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("motionVector"), motionVec->GetSRVHandle());
    commandList->Dispatch((halfRes.cx + 15) / 16, (halfRes.cy + 15) / 16, 1);
}

void GITemporalPass::InitShaderAnsPSO()
{
    ComputePipelineStateStream pss;
    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}