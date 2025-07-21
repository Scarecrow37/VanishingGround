#include "pch.h"
#include "PBRLitTechnique.h"
#include "DeferredPBRLitPass.h"
#include "GBufferPass.h"

PBRLitTechnique::PBRLitTechnique() {}

PBRLitTechnique::~PBRLitTechnique() {}

void PBRLitTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    InitGBufferPass(commandList);
    InitDeferredPass(commandList);
}

void PBRLitTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    __super::Execute(commandList);
}

void PBRLitTechnique::InitGBufferPass(ID3D12GraphicsCommandList* commandList)
{    
    std::unique_ptr<GBufferPass> pass = std::make_unique<GBufferPass>();
    pass->Initialize(_ownerScene, commandList);
    AddRenderPass(std::move(pass));
}

void PBRLitTechnique::InitDeferredPass(ID3D12GraphicsCommandList* commandList)
{    
    std::unique_ptr<DeferredPBRLitPass> pass = std::make_unique<DeferredPBRLitPass>();
    pass->Initialize(_ownerScene, commandList);
    AddRenderPass(std::move(pass));
}
