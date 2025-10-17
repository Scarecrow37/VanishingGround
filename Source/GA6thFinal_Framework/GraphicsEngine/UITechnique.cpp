#include "pch.h"
#include "UITechnique.h"
#include "UI2DPass.h"
#include "UI25DPass.h"
#include "UI3DPass.h"
#include "TextDrawPass.h"
#include "SDFTextDrawPass.h"
#include "SpriteRenderer.h"

UITechnique::UITechnique() = default;

UITechnique::~UITechnique() = default;

void UITechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    _depthStencilView = MakeSharedResource<DepthStencilView>();
    _depthStencilView->Initialize(_ownerScene->_depthStencilView->GetDesc());

    _uiMaterialDataBuffer = std::make_unique<ConstantBufferView>();
    _uiMaterialDataBuffer->Initialize(sizeof(UIMaterialData) * MAX_UI_MATERIAL_DATA);
    
    _uiMaterialDatas.resize(MAX_UI_MATERIAL_DATA);

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

    //pass = std::make_unique<TextDrawPass>();
    //pass->Initialize(_ownerScene, this, commandList);
    //AddRenderPass(std::move(pass));

    /*pass = std::make_unique<SDFTextDrawPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));*/
}

void UITechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    for (auto& data : _renderDatas)
        data.clear();

    UINT index = 0;
    for (auto& component : _ownerScene->_uiRenderQueue)
    {
        if (!component->IsActive())
            continue;

        if (nullptr == component->GetTexture())
            continue;

        SpriteType type = component->GetType();
        _renderDatas[type].push_back(index);
        _uiMaterialDatas[index] = component->GetMaterialData();
        index++;
    }

    _uiMaterialDataBuffer->UpdateBuffer(_uiMaterialDatas.data());

    RenderTechnique::Execute(commandList);
}