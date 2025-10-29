#include "pch.h"
#include "UITechnique.h"
#include "UI2DPass.h"
#include "UI25DPass_OIT.h"
#include "UI3DPass_OIT.h"
#include "SDFTextDrawPass_OIT.h"
#include "SpriteRenderer.h"
#include "SDFTextRenderer.h"
#include "UIResolvePass.h"

UITechnique::UITechnique() = default;

UITechnique::~UITechnique() = default;

void UITechnique::Initialize(ID3D12GraphicsCommandList* commandList)
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
    }

    _nodesBuffer = MakeSharedResource<UnorderedAccessView>();
    {
        _nodesBuffer->InitializeAsStructuredBuffer(MAX_UI_NODES, sizeof(OITNode), true);
    }
    _atomicCounterBuffer = MakeSharedResource<UnorderedAccessView>();
    {
        _atomicCounterBuffer->InitializeAsByteAddressBuffer(sizeof(UINT), true);
    }

    CreateCameraBuffer();

    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<UI2DPass>(&_instanceIDs[MODE_2D]);
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
    
    pass = std::make_unique<UI25DPass_OIT>(&_instanceIDs[MODE_25D]);
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<UI3DPass_OIT>(&_instanceIDs[MODE_3D]);
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));

    /*pass = std::make_unique<SDFTextDrawPass_OIT>(&_instanceIDs[MODE_TEXT]);
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));*/

    pass = std::make_unique<UIResolvePass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}

void UITechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    // Update Instance IDs
    for (auto& data : _instanceIDs)
        data.clear();

    UINT index = 0;
    for (auto& component : _ownerScene->_activeUIs)
    {
        UIType type = component->GetUIType();
        _instanceIDs[type].push_back(index);

        UIMaterialData data{};
        switch (component->GetType())
        {
            case UIRenderer::Type::SPRITE: {
                SpriteRenderer* spriteRenderer = static_cast<SpriteRenderer*>(component);
                data                           = spriteRenderer->GetMaterialData();
                break;
            }
        }

        _uiMaterialDatas[index] = data;
        index++;
    }

    _uiMaterialDataBuffer->UpdateBuffer(_uiMaterialDatas.data());

    // Update Camera
    _cameraData[MODE_25D].Projection = XMMatrixTranspose(_ownerScene->_camera->GetProjectionMatrix());
    _cameraData[MODE_3D].View        = XMMatrixTranspose(_ownerScene->_camera->GetViewMatrix());
    _cameraData[MODE_3D].Projection  = XMMatrixTranspose(_ownerScene->_camera->GetProjectionMatrix());

    _cameraBuffers[MODE_25D]->UpdateBuffer(&_cameraData[MODE_25D]);
    _cameraBuffers[MODE_3D]->UpdateBuffer(&_cameraData[MODE_3D]);

    // Update Default Buffers
    _depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    _nodesBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _headBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
    _atomicCounterBuffer->TransitionResource(commandList, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);

    constexpr UINT value[4] = {UINT_MAX, UINT_MAX, UINT_MAX, UINT_MAX};
    _headBuffer->ClearUnorderedAccessView(commandList, value);
    _atomicCounterBuffer->ClearUnorderedAccessView(commandList, Vector4(0, 0, 0, 0));
    _depthStencilView->ClearDepthStencilView(commandList);

    RenderTechnique::Execute(commandList);
}

void UITechnique::CreateCameraBuffer()
{
    for (auto& buffer : _cameraBuffers)
    {
        buffer = std::make_shared<ConstantBufferView>();
        buffer->Initialize(sizeof(CameraData));
    }

    const auto& size = Global::device->GetResolution();

    _cameraData[MODE_2D].View = XMMatrixTranspose(XMMatrixLookAtLH({0.f, 0.f, -1.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}));
    _cameraData[MODE_2D].Projection = XMMatrixTranspose(XMMatrixOrthographicOffCenterLH(0.f, (float)size.cx, (float)size.cy, 0.f, 0.1f, 1000.f));
    _cameraBuffers[MODE_2D]->UpdateBuffer(&_cameraData[MODE_2D]);

    _cameraData[MODE_25D].View = XMMatrixTranspose(XMMatrixLookAtLH({0.f, 0.f, -1.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}));
    _cameraData[MODE_25D].Projection = XMMatrixIdentity();

    _cameraData[MODE_3D].View = XMMatrixIdentity();
    _cameraData[MODE_3D].Projection = XMMatrixIdentity();
}