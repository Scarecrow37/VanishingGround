#include "pch.h"
#include "RayTracingTechnique.h"
#include "DXRDrawStaticMeshPass.h"
RayTracingTechnique::RayTracingTechnique() {}

RayTracingTechnique::~RayTracingTechnique() {}

void RayTracingTechnique::Initialize(ID3D12GraphicsCommandList* commandList) 
{
    InitDXRDrawStaticMeshPass();
}

void RayTracingTechnique::Execute(ID3D12GraphicsCommandList* commandList) 
{
    UmAccelerationStructureManager.BeginFrame();
    __super::Execute(commandList);
}

void RayTracingTechnique::InitDXRDrawStaticMeshPass() 
{
    std::unique_ptr<DXRDrawStaticMeshPass> pass = std::make_unique<DXRDrawStaticMeshPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize();
    AddRenderPass(std::move(pass));
}
