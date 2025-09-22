#include "pch.h"
#include "UITechnique_OIT.h"
#include "UI2DPass_OIT.h"
#include "UI25DPass_OIT.h"
#include "UI3DPass_OIT.h"
#include "TextDrawPass.h"
#include "SpriteRenderer.h"
#include "UIResolvePass.h"

UITechnique_OIT::UITechnique_OIT() = default;

UITechnique_OIT::~UITechnique_OIT() = default;

void UITechnique_OIT::Initialize(ID3D12GraphicsCommandList* commandList)
{
    _uiMaterialDataBuffer = std::make_unique<ConstantBufferView>();
    _uiMaterialDataBuffer->Initialize(sizeof(UIMaterialData) * MAX_UI_MATERIAL_DATA);
    
    _uiMaterialDatas.resize(MAX_UI_MATERIAL_DATA);

    _depthStencilView = MakeSharedResource<DepthStencilView>();
    _depthStencilView->Initialize(_ownerScene->_depthStencilView->GetDesc());

    const auto&    resolution   = Global::device->GetResolution();
    constexpr UINT DEPTH        = 4;
    const UINT     MAX_UI_NODES = resolution.cx * resolution.cy * DEPTH;

    _headBuffer = MakeSharedResource<UnorderedAccessView>();
    {
        D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32_UINT, resolution.cx, resolution.cy, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);
        _headBuffer->InitializeAsTexture(desc, UnorderedAccessView::UAVSliceType::PER_MIP);
        _headBuffer->SetName(L"Head Buffer");
    }

    _nodesBuffer = MakeSharedResource<UnorderedAccessView>();
    {
        _nodesBuffer->InitializeAsStructuredBuffer(MAX_UI_NODES, sizeof(OITNode), true);
    }
    _atomicCounterBuffer = MakeSharedResource<UnorderedAccessView>();
    {
        _atomicCounterBuffer->InitializeAsByteAddressBuffer(sizeof(UINT), true);
    }

    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<UI2DPass_OIT>(&_renderDatas[MODE_2D]);
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    /*pass = std::make_unique<UI25DPass_OIT>(_renderDatas[MODE_25D]);
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<UI3DPass_OIT>(_renderDatas[MODE_3D]);
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));*/

    pass = std::make_unique<UIResolvePass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<TextDrawPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}

void UITechnique_OIT::Execute(ID3D12GraphicsCommandList* commandList)
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
        _renderDatas[type].push_back(index);
        _uiMaterialDatas[index] = component->GetMaterialData();
        index++;
    }

    _uiMaterialDataBuffer->UpdateBuffer(_uiMaterialDatas.data());

    RenderTechnique::Execute(commandList);
}