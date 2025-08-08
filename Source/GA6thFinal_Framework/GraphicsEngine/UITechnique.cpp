#include "pch.h"
#include "UITechnique.h"
#include "UI2DPass.h"
#include "UI25DPass.h"
#include "UI3DPass.h"
#include "SpriteRenderer.h"

UITechnique::UITechnique() {}

UITechnique::~UITechnique() {}

void UITechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<UI2DPass>(_renderDatas[MODE_2D]);
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<UI25DPass>(_renderDatas[MODE_25D]);
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<UI3DPass>(_renderDatas[MODE_3D]);
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    _depthStencilView = MakeSharedResource<DepthStencilView>();
    _depthStencilView->Initialize(_ownerScene->_depthStencilView->GetDesc());
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

        if (nullptr == component->GetTexture())
            continue;

        SpriteType type = component->GetType();
        _renderDatas[type].push_back(index++);
    }

    __super::Execute(commandList);
}