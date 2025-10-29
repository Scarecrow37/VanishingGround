#include "pch.h"
#include "FXAATechnique.h"
#include "AntiAliasingPass.h"

FXAATechnique::FXAATechnique() = default;

FXAATechnique::~FXAATechnique() = default;

void FXAATechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<RenderPass> pass;
    pass = std::make_unique<AntiAliasingPass>();
    pass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(pass));
}