#include "pch.h"
#include "FadePass.h"
#include "SceneTransitionTechnique.h"

SceneTransitionTechnique::SceneTransitionTechnique() {}

SceneTransitionTechnique::~SceneTransitionTechnique() {}

void SceneTransitionTechnique::Initialize(ID3D12GraphicsCommandList* commandList)
{
    std::unique_ptr<FadePass> fadepass = std::make_unique<FadePass>();
    fadepass->Initialize(_ownerScene, this, commandList);
    AddRenderPass(std::move(fadepass));

}
