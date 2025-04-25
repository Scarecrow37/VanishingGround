#include "pch.h"
#include "PBRLitTechnique.h"
#include "GBufferPass.h"
#include "Shader.h"
PBRLitTechnique::PBRLitTechnique() {}

PBRLitTechnique::~PBRLitTechnique() {}

void PBRLitTechnique::Initialize() 
{
    InitGBufferPass();
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
