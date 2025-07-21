#include "pch.h"
#include "ParticleResolvePass.h"
#include "ParticleSpritePass.h"
#include "ParticleRibbonPass.h"
#include "ParticleRenderTechnique.h"

 ParticleRenderTechnique::ParticleRenderTechnique() {}

 ParticleRenderTechnique::~ParticleRenderTechnique() {}

 void ParticleRenderTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    D3D12_FEATURE_DATA_D3D12_OPTIONS options = {};
    HRESULT hr = Global::device->GetDevice()->CheckFeatureSupport(D3D12_FEATURE_D3D12_OPTIONS, &options, sizeof(options));

    if (!options.ROVsSupported)
    {
        std::filesystem::path errorMessage = static_cast<const char*>(nullptr);
        GRAPHICS_ASSERT(SUCCEEDED(hr), errorMessage.c_str());
    }        

    CreateWBOITResources();
    InitializeSpriteParticlePass(commandList);
    InitializeRibbonParticlePass(commandList);
    InitializeParticleResolvePass(commandList);
}

void ParticleRenderTechnique::Execute(ID3D12GraphicsCommandList* commandList)
{
    Global::particleManager->SetCurrentRenderScene(_ownerScene);
    __super::Execute(commandList);
}

void ParticleRenderTechnique::InitializeSpriteParticlePass(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<ParticleSpritePass> spritepass = std::make_unique<ParticleSpritePass>();
    spritepass->Initialize(_ownerScene, commandList);
    spritepass->SetAccumulationBuffers(_accumlateBuffer, _revealageBuffer);
    AddRenderPass(std::move(spritepass));
}

void ParticleRenderTechnique::InitializeRibbonParticlePass(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<ParticleRibbonPass> ribbonpass = std::make_unique<ParticleRibbonPass>();
    ribbonpass->Initialize(_ownerScene,commandList);
    ribbonpass->SetAccumulationBuffers(_accumlateBuffer, _revealageBuffer);
    AddRenderPass(std::move(ribbonpass));


}

void ParticleRenderTechnique::InitializeParticleResolvePass(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<ParticleResolvePass> resolvepass = std::make_unique<ParticleResolvePass>();
    resolvepass->Initialize(_ownerScene, commandList);
    resolvepass->SetAccumulationBuffers(_accumlateBuffer, _revealageBuffer);
    AddRenderPass(std::move(resolvepass));
}

void ParticleRenderTechnique::CreateWBOITResources()
{
    _accumlateBuffer = MakeSharedResource<UnorderedAccessView>();
    _revealageBuffer = MakeSharedResource<UnorderedAccessView>();

    Global::dxResourceManager->AddResource(_accumlateBuffer);
    Global::dxResourceManager->AddResource(_revealageBuffer);

    DXGI_MODE_DESC mode = Global::device->GetMode();
    mode.Format         = DXGI_FORMAT_R16G16B16A16_FLOAT;
    _accumlateBuffer->Initialize(mode);

    mode.Format = DXGI_FORMAT_R16_FLOAT;
    _revealageBuffer->Initialize(mode);
    _accumlateBuffer->SetName(L"particle accum");
    _revealageBuffer->SetName(L"particle reveal");
}

void ParticleRenderTechnique::ReleaseWBOITResources()
{
}