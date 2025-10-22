#include "pch.h"
#include "GenerateSSGIPass.h"
#include "SSGITechnique.h"

void GenerateSSGIPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
    InitShaderAnsPSO();
    _ssgiTech = dynamic_cast<SSGITechnique*>(ownerTechnique);
}

void GenerateSSGIPass::Begin(ID3D12GraphicsCommandList* commandList) 
{
    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetComputeRootSignature(_fx.GetRootSignature());
}

void GenerateSSGIPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    D3D12_GPU_VIRTUAL_ADDRESS giData       = _ssgiTech->GetConstantBufferView()->GetGPUVirtualAddress();
    D3D12_GPU_VIRTUAL_ADDRESS cameraData   = _ownerScene->_cameraBuffer->GetGPUVirtualAddress();
    int                       currentIndex = _ssgiTech->_currIndex;
    auto                      currGITex    = _ssgiTech->_GIHalf2D[currentIndex];
    const auto&               gbuffers     = Global::multiRenderTargetManager->GetRenderTargetGroup("G-Buffer");
    SIZE                      res          = Global::device->GetResolution();
    SIZE                      halfRes      = SIZE((int)(res.cx / 2.f), (int)(res.cy / 2.f));

    commandList->SetComputeRootConstantBufferView(_fx.GetRootParameterIndex("ssgiData"), giData);
    commandList->SetComputeRootConstantBufferView(_fx.GetRootParameterIndex("cameraData"), cameraData);
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("screenDepth"), gbuffers[GBuffer::DEPTH]->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("screenNormal"), gbuffers[GBuffer::NORMAL]->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("screenAlbedo"), gbuffers[GBuffer::BASECOLOR]->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("screenORM"), gbuffers[GBuffer::ORM]->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("screenColor"), _meshRenderTarget->GetSRVHandle());
    commandList->SetComputeRootDescriptorTable(_fx.GetRootParameterIndex("SSGI_Result"), currGITex->GetUAVHandle());
    commandList->Dispatch((halfRes.cx + 15) / 16, (halfRes.cy + 15) / 16, 1);
}

void GenerateSSGIPass::InitShaderAnsPSO() 
{
    ComputePipelineStateStream pss;
    _fx.SetPipelineStateStream(pss);
    _pipelineState = Global::pipelineStateManager->GetPipelineState(pss);
}