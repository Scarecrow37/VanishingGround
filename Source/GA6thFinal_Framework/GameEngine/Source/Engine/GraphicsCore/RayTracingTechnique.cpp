#include "pch.h"
#include "RayTracingTechnique.h"
#include "DXRDrawStaticMeshPass.h"
#include "GBufferPass.h"

RayTracingTechnique::RayTracingTechnique() {}

RayTracingTechnique::~RayTracingTechnique() {}

void RayTracingTechnique::Initialize(ID3D12GraphicsCommandList* commandList) 
{
    InitGbufferPass();
    InitDXRDrawStaticMeshPass();
}

void RayTracingTechnique::Execute(ID3D12GraphicsCommandList* commandList) 
{
    //UnifiedVIBuffer();
    _ownerScene->_accelerationStructureManager->BeginFrame();
    __super::Execute(commandList);
}

void RayTracingTechnique::InitDXRDrawStaticMeshPass() 
{
    std::unique_ptr<DXRDrawStaticMeshPass> pass = std::make_unique<DXRDrawStaticMeshPass>();
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));
}

void RayTracingTechnique::InitGbufferPass() 
{
    std::unique_ptr<GBufferPass> pass = std::make_unique<GBufferPass>();
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));
}
