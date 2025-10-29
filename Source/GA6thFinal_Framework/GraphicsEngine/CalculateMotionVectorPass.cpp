#include "pch.h"
#include "CalculateMotionVectorPass.h"
#include "ShaderBuilder.h"
#include "SSGITechnique.h"

CalculateMotionVectorPass::~CalculateMotionVectorPass() = default;

void CalculateMotionVectorPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechique, ID3D12GraphicsCommandList* commadList)
{
    RenderPass::Initialize(ownerScene, ownerTechique, commadList);
    InitShaderAndPSO();
    _ssgiTech = dynamic_cast<SSGITechnique*>(ownerTechique);
}

void CalculateMotionVectorPass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetComputeRootSignature(_fx.GetRootSignature());
}

void CalculateMotionVectorPass::Draw(ID3D12GraphicsCommandList* commandList) 
{
    D3D12_GPU_VIRTUAL_ADDRESS giData          = _ssgiTech->GetConstantBufferView()->GetGPUVirtualAddress();
    auto                      motionVectorMap = _ssgiTech->_motionVectorTex2D;
    const auto&               gBuffers        = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");
    auto                      res             = Global::device->GetResolution();

    commandList->SetComputeRootConstantBufferView(_fx.GetRootParameterIndex("SSGIDatas"), giData);
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("motionVector"), motionVectorMap->GetUAVHandle());
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("depthMap"), gBuffers[GBuffer::DEPTH]->GetSRVHandle());
    commandList->Dispatch((res.cx + 15) / 16, (res.cy + 15) / 16,1);
}

void CalculateMotionVectorPass::InitShaderAndPSO()
{
    ComputePipelineStateStream pss;
    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}
