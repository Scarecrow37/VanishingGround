#include "pch.h"
#include "BlendTechnique.h"
#include "BlendPass.h"

BlendTechnique::BlendTechnique() {}

BlendTechnique::~BlendTechnique() {}

void BlendTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    const auto&    mode = UmDevice.GetMode();
    D3D12_VIEWPORT viewport{.Width = (FLOAT)mode.Width, .Height = (FLOAT)mode.Height, .MinDepth = 0.f, .MaxDepth = 1.f};
    D3D12_RECT     scissor{.right = (LONG)mode.Width, .bottom = (LONG)mode.Height};

    std::unique_ptr<RenderPass> pass;    
    pass = std::make_unique<BlendPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize(viewport, scissor);
    AddRenderPass(std::move(pass));
}

void BlendTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    ID3D12GraphicsCommandList* postProcessCommandList = UmDevice.GetPostProcessCommandList();
    auto                       descriptorHeap         = UmViewManager.GetShaderResourceHeap();

    postProcessCommandList->SetDescriptorHeaps(1, &descriptorHeap);

    __super::Execute(postProcessCommandList);
}