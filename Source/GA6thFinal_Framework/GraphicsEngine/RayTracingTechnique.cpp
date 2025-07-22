#include "pch.h"
#include "RayTracingTechnique.h"
#include "DXRDrawPass.h"
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
    _ownerScene->_accelerationStructureManager->BeginFrame();
    __super::Execute(commandList);
}

void RayTracingTechnique::InitDXRDrawStaticMeshPass(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<DXRDrawPass> pass = std::make_unique<DXRDrawPass>();
    pass->Initialize(_ownerScene, commandList);
    AddRenderPass(std::move(pass));
}

void RayTracingTechnique::InitDXRGbufferPass(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<DXRGBufferPass> pass = std::make_unique<DXRGBufferPass>();
    pass->Initialize(_ownerScene, commandList);
    AddRenderPass(std::move(pass));
}
