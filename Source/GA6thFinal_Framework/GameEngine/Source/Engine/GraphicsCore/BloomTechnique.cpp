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

    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<BrightExtractPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize();
    AddRenderPass(std::move(pass));

    pass = std::make_unique<DownScalePass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize();
    AddRenderPass(std::move(pass));

    pass = std::make_unique<UpScalePass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize();
    AddRenderPass(std::move(pass));

    pass = std::make_unique<BlurXPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize();
    AddRenderPass(std::move(pass));

    pass = std::make_unique<BlurYPass>();
    pass->SetOwnerScene(_ownerScene);
    pass->Initialize();
    AddRenderPass(std::move(pass));
}

void BloomTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    ID3D12GraphicsCommandList* postProcessCommandList = UmDevice.GetPostProcessCommandList();
    auto                       descriptorHeap         = UmViewManager.GetShaderResourceHeap();

    postProcessCommandList->SetDescriptorHeaps(1, &descriptorHeap);

    __super::Execute(postProcessCommandList);
}