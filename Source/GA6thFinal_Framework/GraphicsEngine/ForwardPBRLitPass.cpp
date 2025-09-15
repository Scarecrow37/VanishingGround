#include "pch.h"
#include "ForwardPBRLitPass.h"

ForwardPBRLitPass::~ForwardPBRLitPass() = default;

void ForwardPBRLitPass::Initialize(RenderScene* ownerScene, RenderTechnique* ownerTechnique, ID3D12GraphicsCommandList* commandList)
{
}

void ForwardPBRLitPass::Begin(ID3D12GraphicsCommandList* commandList) {}

void ForwardPBRLitPass::Draw(ID3D12GraphicsCommandList* commandList) {}

void ForwardPBRLitPass::End(ID3D12GraphicsCommandList* commandList) {}
