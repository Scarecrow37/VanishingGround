#include "pch.h"
#include "CalculateMotionVectorPass.h"
#include "ShaderBuilder.h"
#include "SSGITechnique.h"

CalculateMotionVectorPass::~CalculateMotionVectorPass() {}

void CalculateMotionVectorPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechique,
                                           ID3D12GraphicsCommandList* commadList)
{
    RenderPass::Initialize(ownerScene, ownerTechique, commadList);
    InitShaderAndPSO();
    _ssgiTech = dynamic_cast<SSGITechnique*>(_ownerTechnique);
}

void CalculateMotionVectorPass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetComputeRootSignature(_shader->GetRootSignature());
}

void CalculateMotionVectorPass::Draw(ID3D12GraphicsCommandList* commandList) 
{
    D3D12_GPU_VIRTUAL_ADDRESS giData = _ssgiTech->GetConstantBufferView()->GetGPUVirtualAddress();
    auto                      motionVectorMap = _ssgiTech->_motionVectorTex2D;
    const auto&               gBuffers        = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");
    auto                      res             = Global::device->GetResolution();
    commandList->SetComputeRootConstantBufferView(_shader->GetRootParameterIndex("SSGIDatas"), giData);
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("motionVector"),
                                               motionVectorMap->GetUAVHandle());
    commandList->SetComputeRootDescriptorTable(_shader->GetRootParameterIndex("depthMap"),
                                               gBuffers[GBuffer::DEPTH]->GetSRVHandle());
    commandList->Dispatch((res.cx + 15) / 16, (res.cy + 15) / 16,1);
}

void CalculateMotionVectorPass::InitShaderAndPSO()
{
    _shader = std::make_unique<ShaderBuilder>();
    _shader->BeginBuild();
    _shader->SetShader(L"../Shader/cs_calculate_motion_vector.hlsl", ShaderBuilder::Type::CS);
    _shader->EndBuild();

    D3D12_COMPUTE_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.pRootSignature = _shader->GetRootSignature();
    psoDesc.CS             = _shader->GetShaderByteCode(ShaderBuilder::Type::CS);
    psoDesc.Flags          = D3D12_PIPELINE_STATE_FLAG_NONE;

    HRESULT hr =
        Global::device->GetDevice()->CreateComputePipelineState(&psoDesc, IID_PPV_ARGS(_pipelineState.GetAddressOf()));
    FAILED_CHECK_MESSAGE(hr, L"CalculateMotionVectorPass::InitShaderAndPSO Failed");
}
