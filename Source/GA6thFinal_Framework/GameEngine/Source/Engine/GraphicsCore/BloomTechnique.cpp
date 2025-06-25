#include "pch.h"
#include "BloomTechnique.h"
#include "BrightExtractPass.h"
#include "DownScalePass.h"
#include "UpScalePass.h"
#include "BlurXPass.h"
#include "BlurYPass.h"

BloomTechnique::BloomTechnique() {}

BloomTechnique::~BloomTechnique() {}

void BloomTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    const auto&    mode = UmDevice.GetMode();
    D3D12_VIEWPORT viewport{.Width = (FLOAT)mode.Width, .Height = (FLOAT)mode.Height, .MinDepth = 0.f, .MaxDepth = 1.f};
    D3D12_RECT     scissor{.right = (LONG)mode.Width, .bottom = (LONG)mode.Height};

    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<BrightExtractPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize(viewport, scissor);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<DownScalePass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize(viewport, scissor);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<UpScalePass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize(viewport, scissor);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<BlurXPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize(viewport, scissor);
    AddRenderPass(std::move(pass));

    pass = std::make_unique<BlurYPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize(viewport, scissor);
    AddRenderPass(std::move(pass));
}

void BloomTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    ID3D12GraphicsCommandList* postProcessCommandList = UmDevice.GetPostProcessCommandList();
    auto                       descriptorHeap         = UmViewManager.GetShaderResourceHeap();

    postProcessCommandList->SetDescriptorHeaps(1, &descriptorHeap);

    __super::Execute(postProcessCommandList);
    }
}