#include "pch.h"
#include "UITechnique.h"
#include "UI2DPass.h"
#include "UI3DPass.h"
#include "UIRenderer.h"

UITechnique::UITechnique() {}

UITechnique::~UITechnique() {}

void UITechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<UI2DPass>(_renderDatas[MODE_2D]);
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));

    /*pass = std::make_unique<UI3DPass>();
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));*/
}

void UITechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    for (auto& data : _renderDatas)
        data.clear();

    UINT index = 0;
    for (auto& [isDestroy, component] : _ownerScene->_uiRenderQueue)
    {
        if (!component->IsActive())
            continue;

        UIType type = component->GetType();
        _renderDatas[type].push_back(index++);
    }

    __super::Execute(commandList);
}