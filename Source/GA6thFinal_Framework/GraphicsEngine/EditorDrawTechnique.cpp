#include "pch.h"
#include "EditorDrawTechnique.h"
#include "EndlessGridPass.h"
#include "OutLinePass.h"

EditorDrawTechnique::EditorDrawTechnique() {}

EditorDrawTechnique::~EditorDrawTechnique() {}

void EditorDrawTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<EndlessGridPass>();
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<OutLinePass>();
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));
}

void EditorDrawTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{    
    __super::Execute(commandList);
}