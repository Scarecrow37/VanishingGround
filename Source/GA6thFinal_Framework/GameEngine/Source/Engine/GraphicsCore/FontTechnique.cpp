#include "pch.h"
#include "FontTechnique.h"
#include "TextDrawPass.h"

FontTechnique::FontTechnique() {}

FontTechnique::~FontTechnique() {}

static std::unique_ptr<SpriteBatch> spriteBatch;

void FontTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    static bool isFirstInit = false;
    if (!isFirstInit)
    {
        try
        {
            auto& device = UmDevice;

            DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
            if constexpr (IS_EDITOR)
                format = DXGI_FORMAT_R32G32B32A32_FLOAT;

            RenderTargetState rtState(format, _ownerScene->_depthStencilView->GetFormat());

            D3D12_BLEND_DESC blendDesc       = {};
            blendDesc.AlphaToCoverageEnable  = FALSE;
            blendDesc.IndependentBlendEnable = FALSE;

            auto& rtDesc                 = blendDesc.RenderTarget[0];
            rtDesc.BlendEnable           = TRUE;
            rtDesc.SrcBlend              = D3D12_BLEND_SRC_ALPHA;
            rtDesc.DestBlend             = D3D12_BLEND_INV_SRC_ALPHA;
            rtDesc.BlendOp               = D3D12_BLEND_OP_ADD;
            rtDesc.SrcBlendAlpha         = D3D12_BLEND_ZERO;
            rtDesc.DestBlendAlpha        = D3D12_BLEND_ONE;
            rtDesc.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
            rtDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

            SpriteBatchPipelineStateDescription psd(rtState, &blendDesc, &CommonStates::DepthRead);
            ResourceUploadBatch                 resourceUpload(device.GetDevice());
            resourceUpload.Begin();

            spriteBatch = std::make_unique<SpriteBatch>(device.GetDevice(), resourceUpload, psd);

            auto uploadFinish = resourceUpload.End(UmCommandController.GetCommandQueue(CommandQueueType::GRAPHICS_QUEUE));
            uploadFinish.wait();
        }
        catch (std::exception& e)
        {
            std::filesystem::path msg = e.what();
            GRAPHICS_ASSERT(false, L"Font creation failed!");
        }

        isFirstInit = true;
    }

    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<TextDrawPass>(spriteBatch.get());
    pass->Initialize(_ownerScene);
    AddRenderPass(std::move(pass));
}

void FontTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    const auto&    mode = UmDevice.GetMode();
    D3D12_VIEWPORT viewport{.Width = (float)mode.Width, .Height = (float)mode.Height, .MaxDepth = 1.f};
    spriteBatch->SetViewport(viewport);

    spriteBatch->Begin(commandList);

    __super::Execute(commandList);

    spriteBatch->End();

    if constexpr (IS_EDITOR)
    {
        RenderTarget* renderTarget = UmMultiRenderTargetManager.GetRenderTarget(_ownerScene->_finalTargetName);
        renderTarget->TransitionResource(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    }
}