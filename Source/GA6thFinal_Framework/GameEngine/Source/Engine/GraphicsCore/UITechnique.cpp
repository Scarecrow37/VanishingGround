#include "pch.h"
#include "UITechnique.h"
#include "UI2DPass.h"
#include "UI3DPass.h"

UITechnique::UITechnique() {}

UITechnique::~UITechnique() {}

void UITechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<UI2DPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize();
    AddRenderPass(std::move(pass));

    pass = std::make_unique<UI3DPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize();
    AddRenderPass(std::move(pass));
}

void UITechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    __super::Execute(commandList);
}