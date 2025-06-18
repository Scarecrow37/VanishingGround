#include "pch.h"
#include "BloomTechnique.h"
#include "BrightExtractPass.h"
#include "DownSamplePass.h"
#include "UpSamplePass.h"
#include "BlendPass.h"

BloomTechnique::BloomTechnique() {}

BloomTechnique::~BloomTechnique() {}

void BloomTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    const auto&    mode = UmDevice.GetMode();
    D3D12_VIEWPORT viewport{.Width = (FLOAT)mode.Width / 4, .Height = (FLOAT)mode.Height / 4, .MinDepth = 0.f, .MaxDepth = 1.f};
    D3D12_RECT     scissor{.right = (LONG)mode.Width / 4, .bottom = (LONG)mode.Height / 4};

    std::shared_ptr<RenderPass> pass = std::make_shared<BrightExtractPass>();
    pass->Initialize(viewport, scissor);
    pass->SetOwnerScene(_ownerScene);
    AddRenderPass(pass);

    pass = std::make_shared<BlendPass>();
    pass->Initialize(viewport, scissor);
    pass->SetOwnerScene(_ownerScene);
    AddRenderPass(pass);

    /*pass = std::make_shared<DownSamplePass>();
    pass->Initialize(viewport, scissor);
    pass->SetOwnerScene(_ownerScene);
    AddRenderPass(pass);

    pass = std::make_shared<UpSamplePass>();
    pass->Initialize(viewport, scissor);
    pass->SetOwnerScene(_ownerScene);
    AddRenderPass(pass);*/
}

void BloomTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    __super::Execute(commandList);
}