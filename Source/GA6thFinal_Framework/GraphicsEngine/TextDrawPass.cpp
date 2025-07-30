#include "pch.h"
#include "TextDrawPass.h"
#include "FontRenderer.h"

TextDrawPass::TextDrawPass(SpriteBatch* spriteBatch)
    : _spriteBatch(spriteBatch)
{
}

TextDrawPass::~TextDrawPass()
{
}

void TextDrawPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
    __super::Initialize(ownerScene, ownerTechnique, commandList);
}

void TextDrawPass::Begin(ID3D12GraphicsCommandList* commandList)
{
    auto& depthStencilView = _ownerScene->_depthStencilView;
    depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_READ);
    
    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, &depthStencilView->GetDSVHandle());    

    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewport());
    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
}

void TextDrawPass::Draw(ID3D12GraphicsCommandList* commandList)
{
    for (auto& [isDestroy, component] : _ownerScene->_fontRenderQueue)
    {
        if (!component->IsActive())
            continue;

        component->Draw(_spriteBatch);
    }
}

void TextDrawPass::End(ID3D12GraphicsCommandList* commandList)
{    
}