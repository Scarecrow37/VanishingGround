#include "pch.h"
#include "ParticleRenderTechnique.h"
#include "ParticleSpritePass.h"
#include "RenderScene.h"
#include "RenderTarget.h"


void ParticleRenderTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    InitializeSpriteParticlePass();

    //ComPtr<ID3D12Resource>   meshRT = _ownerScene->_meshLightingTarget->GetResource();
    //CD3DX12_RESOURCE_BARRIER br     = CD3DX12_RESOURCE_BARRIER::Transition(meshRT.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET,
    //                                                                       D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    //commandList->ResourceBarrier(1, &br);




}

void ParticleRenderTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    __super::Execute(commandList);
    auto particleRenderCmdList = UmParticleManager.GetRenderCommandList().Get();
    particleRenderCmdList->Close();

    UmDevice.RegisterCommand(particleRenderCmdList, COMMAND_LIST_TYPE::PARTICLE_RENDER_LIST);
}

void ParticleRenderTechnique::InitializeSpriteParticlePass()
{
    std::shared_ptr<ParticleSpritePass> spritepass = std::make_shared<ParticleSpritePass>();
    spritepass->SetOwnerScene(_ownerScene);
    D3D12_VIEWPORT viewport{.TopLeftX = 0,
                            .TopLeftY = 0,
                            .Width    = (FLOAT)UmDevice.GetMode().Width,
                            .Height   = (FLOAT)UmDevice.GetMode().Height,
                            .MinDepth = 0.f,
                            .MaxDepth = 1.f};
    D3D12_RECT     scissor{
            .left = 0, .top = 0, .right = (LONG)UmDevice.GetMode().Width, .bottom = (LONG)UmDevice.GetMode().Height};
    spritepass->Initialize(viewport, scissor);
    AddRenderPass(spritepass);
}
