#include "pch.h"
#include "EditorDrawTechnique.h"
#include "EndlessGridPass.h"

EditorDrawTechnique::EditorDrawTechnique() {}

EditorDrawTechnique::~EditorDrawTechnique() {}

void EditorDrawTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    const auto&    mode = UmDevice.GetMode();
    D3D12_VIEWPORT viewport{.Width = (FLOAT)mode.Width, .Height = (FLOAT)mode.Height, .MinDepth = 0.f, .MaxDepth = 1.f};
    D3D12_RECT     scissor{.right = (LONG)mode.Width, .bottom = (LONG)mode.Height};

    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<EndlessGridPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize(viewport, scissor);
    AddRenderPass(std::move(pass));
}

void EditorDrawTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{    
    __super::Execute(commandList);
}