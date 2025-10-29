#include "pch.h"
#include "UI2DPass.h"
#include "UITechnique.h"
#include "FrameResource.h"
#include "SpriteRenderer.h"
#include "SDFTextRenderer.h"
#include "SDFFont.h"

UI2DPass::UI2DPass(const std::vector<UINT>* instanceIDs)
    : UIPassBase(instanceIDs)
{
}

UI2DPass::~UI2DPass() = default;

void UI2DPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    UIPassBase::Initialize(ownerScene, ownerTechnique, commandList);        

    _cameraBuffer = static_cast<UITechnique*>(_ownerTechnique)->GetCameraBuffer(MODE_2D);
}

void UI2DPass::Update(ID3D12GraphicsCommandList* commandList, const float deltaTime)
{
    for (auto& component : _ownerScene->_activeUIs)
    {
        if (UIRenderer::Type::TEXT == component->GetType())
        {
            auto textComponent = static_cast<SDFTextRenderer*>(component);
            textComponent->Update(commandList);
        }
    }
}

void UI2DPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    UINT             instanceCount = 0;
    UINT             instanceID    = 0;
    UINT             offset        = 0;
    UIRenderer::Type previousType  = UIRenderer::Type::NONE;
    UIRenderer*      lastComponent = nullptr;

    for (auto& component : _ownerScene->_activeUIs)
    {
        switch (component->GetType())
        {
        case UIRenderer::Type::SPRITE:
            if (UIRenderer::Type::NONE == previousType)
            {
                previousType  = UIRenderer::Type::SPRITE;
                instanceCount = 1;
            }
            else
            {
                instanceCount++;
            }
            break;
        case UIRenderer::Type::TEXT:
            if (instanceCount > 0) DrawSprite(commandList, offset, instanceCount);
            DrawSDFText(commandList, static_cast<SDFTextRenderer*>(component), instanceID);

            previousType = UIRenderer::Type::NONE;
            offset = instanceID + 1;
            instanceCount = 0;
            break;
        }

        instanceID++;
        lastComponent = component;
    }

    if (lastComponent)
    {
        if (UIRenderer::Type::SPRITE == lastComponent->GetType())
        {
            DrawSprite(commandList, offset, instanceCount);
        }
    }
}

void UI2DPass::DrawSprite(ID3D12GraphicsCommandList* commandList, UINT offset, UINT instanceCount)
{
    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto  resource               = Global::viewManager->GetShaderResourceHeap()->GetGPUDescriptorHandleForHeapStart();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    commandList->SetPipelineState(_pipelineState.Get());
    commandList->SetGraphicsRootSignature(_fx.GetRootSignature());

    frameResource->SetFrameResource(FrameResourceType::UI_TRANSFORM, _fx.GetRootParameterIndex("ui_matrices"), commandList);
    frameResource->SetFrameResource(FrameResourceType::UI_MATERIAL, _fx.GetRootParameterIndex("material"), commandList);

    auto uiMaterialDataBuffer = static_cast<UITechnique*>(_ownerTechnique)->GetUIMaterialDataBuffer();
    commandList->SetGraphicsRoot32BitConstant(_fx.GetRootParameterIndex("bit32_1_offset"), offset, 0);
    commandList->SetGraphicsRootShaderResourceView(_fx.GetRootParameterIndex("uiMaterialData"), uiMaterialDataBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootShaderResourceView(_fx.GetRootParameterIndex("IDs"), _instanceIDBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootConstantBufferView(_fx.GetRootParameterIndex("cameraData"), _cameraBuffer->GetGPUVirtualAddress());
    commandList->SetGraphicsRootDescriptorTable(_fx.GetRootParameterIndex("textures"), resource);

    _halfQuad->Render(commandList, instanceCount);
}

void UI2DPass::DrawSDFText(ID3D12GraphicsCommandList* commandList, SDFTextRenderer* component, UINT instanceID)
{
    UINT  currentBackBufferIndex = Global::device->GetCurrentBackBufferIndex();
    auto& frameResource          = _ownerScene->_frameResources[currentBackBufferIndex];

    commandList->SetPipelineState(_sdfPipelineState.Get());
    commandList->SetGraphicsRootSignature(_fxSDF.GetRootSignature());
    commandList->SetGraphicsRootConstantBufferView(_fxSDF.GetRootParameterIndex("cameraData"), _cameraBuffer->GetGPUVirtualAddress());
    
    frameResource->SetFrameResource(FrameResourceType::UI_TRANSFORM, _fxSDF.GetRootParameterIndex("ui_matrices"), commandList);
            
    const SDFFont* font      = component->GetFont();
    const auto&    atlasInfo = font->GetAtlasInfo();
    const auto&    outline   = component->GetFontOutline();

    SDFParams sdfParams{.InstanceID    = instanceID,
                        .Flags         = component->GetFontFlags(),
                        .DistanceRange = atlasInfo.DistanceRange,
                        .FontWeight    = component->GetFontWeight(),
                        .OutlineColor  = outline.Color,
                        .OutlineWidth  = outline.Width};

    commandList->SetGraphicsRoot32BitConstants(_fxSDF.GetRootParameterIndex("bit32_4_fontColor"), 4, &component->GetColor(), 0);
    commandList->SetGraphicsRoot32BitConstants(_fxSDF.GetRootParameterIndex("bit32_9_sdfParams"), 9, &sdfParams, 0);
    commandList->SetGraphicsRootDescriptorTable(_fxSDF.GetRootParameterIndex("sdfTexture"), component->GetFontTextureHandle());

    component->Render(commandList);
}
