#include "pch.h"
#include "RayTracingTechnique.h"
#include "DXRDrawStaticMeshPass.h"
#include "DXRGBufferPass.h"

RayTracingTechnique::RayTracingTechnique() {}

RayTracingTechnique::~RayTracingTechnique() {}

void RayTracingTechnique::Initialize(ID3D12GraphicsCommandList* commandList) 
{
    InitDXRGbufferPass(commandList);
    InitDXRDrawStaticMeshPass(commandList);
}

void RayTracingTechnique::Execute(ID3D12GraphicsCommandList* commandList) 
{
    //UnifiedVIBuffer();
    _ownerScene->_accelerationStructureManager->BeginFrame();
    __super::Execute(commandList);
}

void RayTracingTechnique::InitDXRDrawStaticMeshPass(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<DXRDrawStaticMeshPass> pass = std::make_unique<DXRDrawStaticMeshPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}

void RayTracingTechnique::InitDXRGbufferPass(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<DXRGBufferPass> pass = std::make_unique<DXRGBufferPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}
