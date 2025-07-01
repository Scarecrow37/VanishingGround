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
    __super::Execute(commandList);
}

void RayTracingTechnique::InitDXRDrawStaticMeshPass() 
{
    const auto&    mode = UmDevice.GetMode();
    D3D12_VIEWPORT viewport{.Width = (FLOAT)mode.Width, .Height = (FLOAT)mode.Height, .MinDepth = 0.f, .MaxDepth = 1.f};
    D3D12_RECT     scissor{.right = (LONG)mode.Width, .bottom = (LONG)mode.Height};
    
    std::unique_ptr<DXRDrawStaticMeshPass> pass = std::make_unique<DXRDrawStaticMeshPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize(viewport, scissor);
    AddRenderPass(std::move(pass));
}
