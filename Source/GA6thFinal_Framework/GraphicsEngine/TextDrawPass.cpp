#include "pch.h"
#include "TextDrawPass.h"
#include "TextRenderer.h"
#include "UITechnique.h"

//static std::unique_ptr<SpriteBatch> spriteBatch;
//
//TextDrawPass::TextDrawPass() = default;
//
//TextDrawPass::~TextDrawPass() = default;
//
//void TextDrawPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
//{
//    RenderPass::Initialize(ownerScene, ownerTechnique, commandList);
//
//    static bool isFirstInit = false;
//    if (!isFirstInit)
//    {
//        try
//        {
//            auto& device = Global::device;
//
//            RenderTargetState rtState(DXGI_FORMAT_R32G32B32A32_FLOAT, static_cast<UITechnique*>(_ownerTechnique)->GetDepthStencilView()->GetFormat());
//
//            D3D12_BLEND_DESC blendDesc       = {};
//            blendDesc.AlphaToCoverageEnable  = FALSE;
//            blendDesc.IndependentBlendEnable = FALSE;
//
//            auto& rtDesc                 = blendDesc.RenderTarget[0];
//            rtDesc.BlendEnable           = TRUE;
//            rtDesc.SrcBlend              = D3D12_BLEND_SRC_ALPHA;
//            rtDesc.DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
//            rtDesc.BlendOp               = D3D12_BLEND_OP_ADD;
//            rtDesc.SrcBlendAlpha         = D3D12_BLEND_ONE;
//            rtDesc.DestBlendAlpha        = D3D12_BLEND_INV_SRC_ALPHA;
//            rtDesc.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
//            rtDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//
//            SpriteBatchPipelineStateDescription psd(rtState, &blendDesc, &CommonStates::DepthRead);
//            ResourceUploadBatch                 resourceUpload(device->GetDevice());
//            resourceUpload.Begin();
//
//            spriteBatch = std::make_unique<SpriteBatch>(device->GetDevice(), resourceUpload, psd);
//
//            auto uploadFinish = resourceUpload.End(Global::commandController->GetCommandQueue(CommandQueueType::GRAPHICS_QUEUE));
//            uploadFinish.wait();
//        }
//        catch (std::exception& e)
//        {
//            std::filesystem::path msg = e.what();
//            GRAPHICS_ASSERT(false, L"Font creation failed!");
//        }
//
//        isFirstInit = true;
//    }
//}
//
//void TextDrawPass::Begin(ID3D12GraphicsCommandList* commandList)
//{
//    const auto&    resolution = Global::device->GetResolution();
//    D3D12_VIEWPORT viewport{.Width = (float)resolution.cx, .Height = (float)resolution.cy, .MaxDepth = 1.f};
//
//    spriteBatch->SetViewport(viewport);
//    spriteBatch->Begin(commandList);
//
//    auto depthStencilView = static_cast<UITechnique*>(_ownerTechnique)->GetDepthStencilView();
//    depthStencilView->TransitionResource(commandList, D3D12_RESOURCE_STATE_DEPTH_READ);
//
//    _finalRenderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET);
//    commandList->OMSetRenderTargets(1, &_finalRenderTarget->GetRTVHandle(), FALSE, &depthStencilView->GetDSVHandle());
//
//    commandList->RSSetViewports(1, &_finalRenderTarget->GetViewport());
//    commandList->RSSetScissorRects(1, &_finalRenderTarget->GetScissorRect());
//}
//
//void TextDrawPass::Draw(ID3D12GraphicsCommandList* commandList)
//{    
//    for (auto& component : _ownerScene->_textRenderQueue)
//    {
//        if (!component->IsActive())
//            continue;
//
//        component->Draw(spriteBatch.get());
//    }
//}
//
//void TextDrawPass::End(ID3D12GraphicsCommandList* commandList)
//{
//    spriteBatch->End();
//
//    RenderTarget* renderTarget = Global::multiRenderTargetManager->GetRenderTarget(_ownerScene->_finalTargetName);
//    renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
//}