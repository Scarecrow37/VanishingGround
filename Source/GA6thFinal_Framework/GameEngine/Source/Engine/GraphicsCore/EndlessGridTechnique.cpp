#include "pch.h"
#include "EndlessGridTechnique.h"
#include "GridPass.h"

EndlessGridTechnique::EndlessGridTechnique() {}

EndlessGridTechnique::~EndlessGridTechnique() {}

void EndlessGridTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    const auto&    mode = UmDevice.GetMode();
    D3D12_VIEWPORT viewport{.Width = (FLOAT)mode.Width, .Height = (FLOAT)mode.Height, .MinDepth = 0.f, .MaxDepth = 1.f};
    D3D12_RECT     scissor{.right = (LONG)mode.Width, .bottom = (LONG)mode.Height};

    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<GridPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize(viewport, scissor);
    AddRenderPass(std::move(pass));
}

void EndlessGridTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{    
    __super::Execute(commandList);
}