#include "pch.h"
#include "PBRLitTechnique.h"
#include "DeferredPBRLitPass.h"
#include "GBufferPass.h"

PBRLitTechnique::PBRLitTechnique() {}

PBRLitTechnique::~PBRLitTechnique() {}

void PBRLitTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    InitGBufferPass();
    InitDeferredPass();    
}

void PBRLitTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    __super::Execute(commandList);
}

void PBRLitTechnique::InitGBufferPass()
{    
    std::unique_ptr<GBufferPass> pass = std::make_unique<GBufferPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize();
    AddRenderPass(std::move(pass));
}

void PBRLitTechnique::InitDeferredPass()
{    
    std::unique_ptr<DeferredPBRLitPass> pass = std::make_unique<DeferredPBRLitPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize();
    AddRenderPass(std::move(pass));
}
