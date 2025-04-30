#include "pch.h"
#include "PBRLitTechnique.h"
#include "GBufferPass.h"
#include "Shader.h"
#include "RenderScene.h"
#include "RenderTarget.h"

PBRLitTechnique::PBRLitTechnique() {}

PBRLitTechnique::~PBRLitTechnique() {}

void PBRLitTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    InitGBufferPass();
    // gbuffer 상태 전이 하기.
    for (UINT i = 0; i < _ownerScene->_gBufferCount; ++i)
    {
        ComPtr<ID3D12Resource> gbuffer = _ownerScene->_gBuffer[i]->GetResource();

        CD3DX12_RESOURCE_BARRIER br = CD3DX12_RESOURCE_BARRIER::Transition(gbuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
                                                                           D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        commandList->ResourceBarrier(1, &br);
    }
    // mesh lighting target 상태 전이하기.
    ComPtr<ID3D12Resource> meshRT = _ownerScene->_meshLightingTarget->GetResource();
    CD3DX12_RESOURCE_BARRIER br     = CD3DX12_RESOURCE_BARRIER::Transition(meshRT.Get(), D3D12_RESOURCE_STATE_COMMON,
                                                                           D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &br);
}

void PBRLitTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    __super::Execute(commandList);

}

void PBRLitTechnique::InitGBufferPass()
{
    std::shared_ptr<GBufferPass> gBufferPass = std::make_shared<GBufferPass>();
    gBufferPass->SetOwnerScene(_ownerScene);
    gBufferPass->SetClearValue(Color(0.f,0.f,0.f,1.f));
    D3D12_VIEWPORT viewport{.TopLeftX = 0,
                            .TopLeftY = 0,
                            .Width    = (FLOAT)UmDevice.GetMode().Width,
                            .Height   = (FLOAT)UmDevice.GetMode().Height,
                            .MinDepth = 0.f,
                            .MaxDepth = 1.f};
    D3D12_RECT     scissor{
            .left = 0, .top = 0, .right = (LONG)UmDevice.GetMode().Width, .bottom = (LONG)UmDevice.GetMode().Height};
    gBufferPass->Initialize(viewport, scissor);
    AddRenderPass(gBufferPass);


}

void PBRLitTechnique::InitDeferredPass() 
{

}
