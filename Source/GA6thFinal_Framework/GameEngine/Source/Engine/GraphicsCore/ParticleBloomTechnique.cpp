#include "pch.h"
#include "ParticleBloomTechnique.h"
#include "ParticleBrightExtractPass.h"
#include "DownScalePass.h"
#include "UpScalePass.h"
#include "BlurXPass.h"
#include "BlurYPass.h"

 ParticleBloomTechnique::ParticleBloomTechnique() {}

 ParticleBloomTechnique::~ParticleBloomTechnique() {}

 void ParticleBloomTechnique::Initialize(ID3D12GraphicsCommandList* commandList) 
 {
     std::unique_ptr<RenderPass> pass;
     pass = std::make_unique<ParticleBrightExtractPass>();
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

void ParticleBloomTechnique::Execute(ID3D12GraphicsCommandList* commandList) 
{
    ID3D12GraphicsCommandList* postProcessCommandList = UmDevice.GetPostProcessCommandList();
    auto                       descriptorHeap         = UmViewManager.GetShaderResourceHeap();

    postProcessCommandList->SetDescriptorHeaps(1, &descriptorHeap);

    __super::Execute(postProcessCommandList);
}
