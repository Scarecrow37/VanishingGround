#include "pch.h"
#include "RayTracingTechnique.h"
#include "DXRGBufferPass.h"
#include "DXRComputeSkeletalMeshPass.h"
#include "ShadowMapPass.h"
#include "DXRDrawPass.h"

RayTracingTechnique::RayTracingTechnique() = default;

RayTracingTechnique::~RayTracingTechnique() = default;

void RayTracingTechnique::Initialize(ID3D12GraphicsCommandList* commandList) 
{
    std::unique_ptr<RenderPass> pass;

    pass = std::make_unique<DXRGBufferPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<DXRComputeSkeletalMeshPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<ShadowMapPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<DXRDrawPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}

void RayTracingTechnique::Execute(ID3D12GraphicsCommandList* commandList) 
{
    _ownerScene->_accelerationStructureManager->BeginFrame();
    RenderTechnique::Execute(commandList);
}